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
			local AnimClipsRollout
			
			parameters AnimClipsparams rollout:AnimClipsRollout
			(
				
				saveSelectedAnimClip type:#integer default:1
				saveAnimClipNames type:#stringTab tabSize:0 tabSizeVariable:true
				saveAnimSourceIDs type:#stringTab tabSize:0 tabSizeVariable:true
				saveStartFrames type:#intTab tabSize:0 tabSizeVariable:true
				saveEndFrames type:#intTab tabSize:0 tabSizeVariable:true
				saveSkipFrames type:#intTab tabSize:0 tabSizeVariable:true
				saveStitchFinals type:#boolTab tabSize:0 tabSizeVariable:true
				
			) 
			rollout AnimClipsRollout "Animation Clips" category:2
			(				
				label lbl_listHeader " Name                         Start       End"     pos:[8,10]
				listbox targetAnimClipsList "" items:#()  os:[6,28]
				button btn_newFromRange "New Clip from Range" width:135 across:2
				button btn_SetRange "Set Range from Clip" width:135 
				button btn_SetRangetoClip "Set Range to Clip" width:135 across:2
				button btn_AWDClipManager "ClipManager" width:135 
				
				on btn_AWDClipManager pressed do(					
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(						
						global thisSourceObj = mtl
						global openAWDClipManager
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						openAWDClipManager mtl "UV" targetAnimClipsList.selection
					)
				)
				on cbx_anim_only changed curState do(
					if curState then (
						SPN_jointPerVert.enabled=false
					)
					else (
						SPN_jointPerVert.enabled=true
					)
				)			
				on btn_newFromRange pressed do (
					firstframe=0
					lastframe=0
					valid=false
					try(
						append saveStartFrames animationRange.start
						append saveEndFrames animationRange.end
						append saveAnimClipNames "new clip"
						append saveAnimSourceIDs (getAWDID() as string)
						append saveSkipFrames 0
						append saveStitchFinals true
						valid=true
					)
					catch()
					if not valid then messageBox("Error - clip was not added")		
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDTextureSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						updateUI mtl targetAnimClipsList.selection
						targetAnimClipsList.selection=(targetAnimClipsList.items.count)
						saveSelectedAnimClip=targetAnimClipsList.selection
					)
					
				)
				on btn_SetRange pressed do (
					firstframe=0
					lastframe=0
					valid=false
					try(
						firstframe=saveStartFrames[targetAnimClipsList.selection]
						lastframe=saveEndFrames[targetAnimClipsList.selection]
						valid=true
					)
					catch()
					if firstframe<lastframe and valid then animationRange = interval firstframe lastframe --Set range to frames 10 to 50
					else messageBox("Invalid time-range set for the selected clip")
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDTextureSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						updateUI mtl (targetAnimClipsList.selection-1)
					)
				)
				on btn_SetRangetoClip pressed do (
						
					valid=false
					try(
						saveStartFrames[targetAnimClipsList.selection]=animationRange.start
						saveEndFrames[targetAnimClipsList.selection]=animationRange.end
						valid=true
					)
					catch()
					if not valid then messageBox("Error - time range was not set for the clip")
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDTextureSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						updateUI mtl (targetAnimClipsList.selection-1)
					)
				)
				on AnimClipsRollout open do (	
					curMod=modPanel.GetCurrentObject()
					--updateUI curMod (saveSelectedAnimClip-1)
				)
			)
			parameters AWD_TextureSettingsparams rollout:AWD_TextureSettings
			(
				thisAWDID type:#String animateable:false
				hasUV type:#boolean default:false
				simpleMode type:#Boolean default:true ui:cbx_simpleMode
				hasObjSettings type:#boolean default:true
				saveTexType type:#integer ui:ddl_saveMode1 default:1
				saveReplace type:#boolean ui:cbx_ReplaceOnExport
				saveReplaceURL type:#string ui:txt_ReplaceOnExportPath default:""
				uvanimeabled type:#boolean ui:cbxenable_uvanim default:false animateable:false
			)
			rollout AWD_TextureSettings "AWD Texture Settings" category:1 (
				label lbl_saveMode "Save Mode:" pos:[10,12] width:80
				dropdownlist ddl_saveMode1 ""  items:#("Use Globale Settings", "External File (Relative)", "External File (Absolute)","Embed Bitmap") pos:[70,8] width:130 height:5
				group "" (
					checkBox cbx_ReplaceOnExport "Replace on Export (for example with ATF)"
					editText txt_ReplaceOnExportPath "Path" readOnly:true pos:[15,77] width:240 
					button txt_BrowsePath "Browse" pos:[265,75] width:50
				)
				label lbl_UVAnim "UV Animation" pos:[40,120] animateable:false
				checkbox cbxenable_uvanim "" pos:[10,120] animateable:false
				checkbox cbx_simpleMode "Auto create anim-setup" pos:[150,120] animateable:false
				
				on cbxenable_uvanim changed thisState do (	
					hasUV=uvanimeabled		
					lbl_UVAnim.enabled=false	
					removeRollout AnimClipsRollout		
					cbx_simpleMode.enabled=false				
					if uvanimeabled then (
						--cbx_simpleMode.enabled=true		
						lbl_UVAnim.enabled=true		
						addRollout AnimClipsRollout	
					)
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
				)	
				
				on AWD_TextureSettings changed thisState do (		
					lbl_UVAnim.enabled=false	
					removeRollout AnimClipsRollout					
					if uvanimeabled then (
						lbl_UVAnim.enabled=true		
						addRollout AnimClipsRollout	
					)
				)
				on AWD_TextureSettings open do (		
					cbx_simpleMode.enabled=false	
					lbl_UVAnim.enabled=false	
					try (removeRollout AnimClipsRollout	)catch()			
					if uvanimeabled then (
						--cbx_simpleMode.enabled=true		
						lbl_UVAnim.enabled=true		
						try (addRollout AnimClipsRollout	)catch()	
					)
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDTextureSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						updateUI mtl (AnimClipsRollout.targetAnimClipsList.selection-1)
					)
				)
			)
		)
		custattributes.add thisTex AWDAttributesCA baseobject:true
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisTex.thisAWDID=getAWDID()
	)	
	return true	
)