
-- add AWDObjectSettings to an object
fn addAWDTextureSettings thisTex = (	
	if classof thisTex != BitmapTexture then return false	
	hasSettings=undefined
	try hasSettings=thisTex.hasObjSettings
	catch (	hasSettings=undefined	)
	if hasSettings!=undefined then (
		return false
	)
	hasSettings=0
	try hasSettings=thisTex.hasObjSettings
	catch hasSettings=0
	if hasSettings==1 then (
		return false
	)
	else (
		AWDAttributesCA = Attributes AWDTextureSettings
		(
			parameters AWD_TextureSettingsparams rollout:AWD_TextureSettings
			(
				hasObjSettings type:#boolean default:true
				saveTexType type:#integer ui:ddl_saveMode1 default:1
				saveReplace type:#boolean ui:cbx_ReplaceOnExport
				saveReplaceURL type:#string ui:txt_ReplaceOnExportPath default:""
			)
			rollout AWD_TextureSettings "AWD Texture Settings" (
				label lbl_saveMode "Save Mode:" pos:[10,12] width:80
				dropdownlist ddl_saveMode1 ""  items:#("Use Globale Settings", "External File (Relative)", "External File (Absolute)","Embed Bitmap") pos:[70,8] width:130 height:5
				group "" (
					checkBox cbx_ReplaceOnExport "Replace on Export (for example with ATF)"
					editText txt_ReplaceOnExportPath "Path" readOnly:true pos:[15,77] width:240 
					button txt_BrowsePath "Browse" pos:[265,75] width:50
				)
				
				on cbx_ReplaceOnExport changed thisValue do (	
					if thisValue then (
						if saveReplaceURL=="" then (
							newfile = getOpenFileName  caption:"Select a texture file" initialDir:#images
							saveReplaceURL=(newfile as string)
						)
					)
				)
				on txt_BrowsePath pressed do (	
					newfile = getOpenFileName  caption:"Select a texture file" initialDir:#images
					saveReplace=true
					saveReplaceURL=(newfile as string)
					--TODO: check if the file exists !
				)	
			)
		)
		custattributes.add thisTex AWDAttributesCA baseobject:true
	)	
	return true	
)