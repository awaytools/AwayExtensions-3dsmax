plugin material AWDCubeMaterial
name:"AWDCubeMaterial"
classID:#(695425, 0x77f56c1a)
extends:MultiMaterial replaceUI:false version:1
(
	fn getUsersForCubeTex thisCubeTex = (		
			global users=#()
			global userDispay=#()
			global userType=#()
			for oneObj in objects do (
				if classof oneObj as string == "AWDSkyBox" then (
					if oneObj.material==thisCubeTex then(
						isadded=appendIfUnique users oneObj
						if isadded then (
							append userDispay ("AWDSkyBox: '"+oneObj.name+"'")
							append userType "AWDSkyBox"
						)
					)
				)
				for oneMod in oneObj.modifiers do(
					if classof oneMod as String == "AWDEffectMethod" then (
						if oneMod.envMap==thisCubeTex then (
							isadded=appendIfUnique users oneMod
							if isadded then (
								append userDispay ("Object: '"+oneObj.name+"' - AWDEffectMethod-modifier: "+oneMod.name+" - EnvMap")
								append userType "AWDEffectMethod"
							)
						)	
						if oneMod.refract_envMap==thisCubeTex then (
							isadded=appendIfUnique users oneMod
							if isadded then (
								append userDispay ("Object: '"+oneObj.name+"' - AWDEffectMethod-modifier: "+oneMod.name+" - RefractionEnvMap")
								append userType "AWDEffectMethod"
							)
						)	
						if oneMod.fresnel_EnvMap==thisCubeTex then (
							isadded=appendIfUnique users oneMod
							if isadded then (
								append userDispay ("Object: '"+oneObj.name+"' - AWDEffectMethod-modifier: "+oneMod.name+" - FresnellEnvMap")
								append userType "AWDEffectMethod"
							)
						)								
					)
				)
			)
			for mat in meditMaterials do(
				if classof mat==Standardmaterial then(
					hasSettings=undefined
					try hasSettings=mat.hasObjSettings
					catch (	hasSettings=undefined	)
					if hasSettings!=undefined then (
						if mat.ambient_envMap==thisCubeTex then (
							isadded=appendIfUnique users mat
							if isadded then (
								append userDispay ("Material: '"+mat.name+"' - AmbientEnvMap")
								append userType "AWDMaterial"
							)		
						)
					)
				)
				else if classof mat==Multimaterial then(
					for matCnt in mat.materialIdList do (
						submat = mat[matCnt]
						if submat != undefined then (
							if classof submat == Standardmaterial then (									
								hasSettings=undefined
								try hasSettings=submat.hasObjSettings
								catch (	hasSettings=undefined	)
								if hasSettings!=undefined then (
									if submat.ambient_envMap==thisCubeTex then (
										isadded=appendIfUnique users submat
										if isadded then (
											append userDispay ("Material: '"+submat.name+"' - AmbientEnvMap")
											append userType "AWDMaterial"
										)		
									)
								)
							)
						)
					)
				)
			)
			for mat in scenematerials do(
				if classof mat==Standardmaterial then(
					hasSettings=undefined
					try hasSettings=mat.hasObjSettings
					catch (	hasSettings=undefined	)
					if hasSettings!=undefined then (
						if mat.ambient_envMap==thisCubeTex then (
							isadded=appendIfUnique users mat
							if isadded then (
								append userDispay ("Material: '"+mat.name+"' - AmbientEnvMap")
								append userType "AWDMaterial"
							)		
						)
					)
				)
				else if classof mat==Multimaterial then(
					for matCnt in mat.materialIdList do (
						submat = mat[matCnt]
						if submat != undefined then (
							if classof submat == Standardmaterial then (									
								hasSettings=undefined
								try hasSettings=submat.hasObjSettings
								catch (	hasSettings=undefined	)
								if hasSettings!=undefined then (
									if submat.ambient_envMap==thisCubeTex then (
										isadded=appendIfUnique users submat
										if isadded then (
											append userDispay ("Material: '"+submat.name+"' - AmbientEnvMap")
											append userType "AWDMaterial"
										)		
									)
								)
							)
						)
					)
				)
			)
			--find all CubeMaterials in the views of the slate-material-editor
			numViews = sme.GetNumViews()
			for numView=1 to numViews do (
				oneView=sme.GetView numView
				numNodes = oneView.GetNumNodes()
				for numNode=1 to numNodes do(
					mat= trackViewNodes[#sme][numView][numNode].reference
					if classof mat==Standardmaterial then(
						hasSettings=undefined
						try hasSettings=mat.hasObjSettings
						catch (	hasSettings=undefined	)
						if hasSettings!=undefined then (
							if mat.ambient_envMap==thisCubeTex then (
								isadded=appendIfUnique users mat
								if isadded then (
									append userDispay ("Material: '"+mat.name+"' - AmbientEnvMap")
									append userType "AWDMaterial"
								)		
							)
						)
					)
					else if classof mat==Multimaterial then(
						for matCnt in mat.materialIdList do (
							submat = mat[matCnt]
							if submat != undefined then (
								if classof submat == Standardmaterial then (									
									hasSettings=undefined
									try hasSettings=submat.hasObjSettings
									catch (	hasSettings=undefined	)
									if hasSettings!=undefined then (
										if submat.ambient_envMap==thisCubeTex then (
											isadded=appendIfUnique users submat
											if isadded then (
												append userDispay ("Material: '"+submat.name+"' - AmbientEnvMap")
												append userType "AWDMaterial"
											)		
										)
									)
								)
							)
						)
					)
				)
			)	

		if users.count>0 then(
			rollout select_users_rollout "AWDCubeMaterial Users"	(
				listbox base_name "Users of selected AWDCubeMaterial"  height:16 items:userDispay
				on base_name selected selected do (
					if userType[base_name.selection]=="AWDSkyBox" then (
						select users[base_name.selection]
					)
					if userType[base_name.selection]=="AWDEffectMethod" then (
						max modify mode 
						modPanel.setCurrentObject users[base_name.selection]
					)
					if userType[base_name.selection]=="AWDMaterial" then (
						if MatEditor.mode ==#basic then 	meditMaterials[activeMeditSlot]=users[base_name.selection]
						else if MatEditor.mode ==#advanced then 	(
							thisNumViews = sme.GetNumViews()
							isFound=false
							for thisNumView=1 to thisNumViews do (
								oneView=sme.GetView thisNumView
								thisNumNodes = oneView.GetNumNodes()
								for thisNumNode=1 to thisNumNodes do(
									thisMatNode= trackViewNodes[#sme][thisNumView][thisNumNode].reference								
									if thisMatNode == users[base_name.selection] then (
										sme.activeView=thisNumView
										isFound=true
										exit									
									)
								)
								if isFound then exit
							)
							if not isFound then (
								cleanNameBool=false
								cleanNameCnt =  1
								cleanName = "AWDMaterial"
								mainName = "AWDMaterial"
								while not cleanNameBool do (
									thisView=sme.GetViewByName cleanName
									if thisView<=0 then cleanNameBool=true
									else (
										cleanName=mainName+"_"+(cleanNameCnt as string)
										cleanNameCnt+=1
									)				
								)
								sme.CreateView cleanName
								thisView=sme.GetView (sme.GetNumViews() as integer)
								sme.activeView=(sme.GetNumViews() as integer)
								thisView.CreateNode users[base_name.selection] [0,0]								
							)
						)
					)
						
					DestroyDialog select_users_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_users_rollout 500 240  modal:true
		)
		else	(
			messagebox("This AWDCubeMaterial is not assigned to any SkyBox, EffectMethod or Material")
		)			
	)
	
	fn loadCubeMapDirectory thisTex = (
		newfile=undefined
		newfile = getOpenFileName   caption:"Select one of the 6 bitmaps that make up a Cubetexture.\n The names of the 6 Bitmaps should only differ in their suffix ('_posX','_posY', '_posZ', '_negX', '_negY', '_negZ')" initialDir:#images
		if newfile!=undefined then (
			newStringList=filterString (newfile as string) "." 
			newName=newStringList[1]
			newerNameList=filterString (newName as string) "_" splitEmptyTokens:true
			newerName=newerNameList[1]
			if newerNameList.Count>2 then(
				for i=2 to newerNameList.Count-1 do(
					newerName+="_"+newerNameList[i]
				)
			)			
			cntAll=0
			posYPath = newerName as string +"_posY."+newStringList[2] as string 
			hasPosY = ((getfiles posYPath).count != 0)
			if hasPosY then (
				cntAll+=1
				thisTex.upSave=bitmaptexture()
				thisTex.upSave.filename=posYPath				
			)
			negYPath = newerName as string +"_negY."+newStringList[2] as string 
			hasNegY = ((getfiles negYPath).count != 0)
			if hasNegY then (
				cntAll+=1
				thisTex.downSave=bitmaptexture()
				thisTex.downSave.filename=negYPath				
			)
			posXPath = newerName as string +"_posX."+newStringList[2] as string 
			hasPosX = ((getfiles posXPath).count != 0)
			if hasPosX then (
				cntAll+=1
				thisTex.leftSave=bitmaptexture()
				thisTex.leftSave.filename=posXPath				
			)
			negXPath = newerName as string +"_negX."+newStringList[2] as string 
			hasNegX = ((getfiles negXPath).count != 0)
			if hasNegX  then (
				cntAll+=1
				thisTex.rightSave=bitmaptexture()
				thisTex.rightSave.filename=negXPath 				
			)
			posZPath = newerName as string +"_posZ."+newStringList[2] as string 
			hasPosZ = ((getfiles posZPath).count != 0)
			if hasPosZ then (
				cntAll+=1
				thisTex.frontSave=bitmaptexture()
				thisTex.frontSave.filename=posZPath				
			)
			negZPath = newerName as string +"_negZ."+newStringList[2] as string 
			hasNegZ = ((getfiles negZPath).count != 0)
			if hasNegZ then (
				cntAll+=1
				thisTex.backSave=bitmaptexture()
				thisTex.backSave.filename=negZPath				
			)
			if (cntAll<6) then messageBox("Could not find 6 Bitmaps that are matching the naming convenitions for CubeTexture. \nThe names of the 6 Bitmaps should only differ in their suffix ('_posX','_posY', '_posZ', '_negX', '_negY', '_negZ')")
			
		)
		
	)
  parameters main rollout:params
  (
	upSave type:#texturemap ui:mbtn_up
	downSave type:#texturemap ui:mbtn_down
	rightSave type:#texturemap ui:mbtn_right
	leftSave type:#texturemap ui:mbtn_left
	frontSave type:#texturemap ui:mbtn_front
	backSave type:#texturemap ui:mbtn_back
	saveTexType type:#integer ui:ddl_saveMode1 default:1
	saveReplace type:#boolean ui:cbx_ReplaceOnExport default:false
	saveReplaceURL type:#string ui:txt_ReplaceOnExportPath default:""
	  
    on upSave set val do (		
		delegate.materialList[1].diffuseMap = val		
		showTextureMap delegate.materialList[1] on)
    on downSave set val do  (
		delegate.materialList[2].diffuseMap = val		
		showTextureMap delegate.materialList[2] on)
    on rightSave set val do (
		delegate.materialList[3].diffuseMap = val
		showTextureMap delegate.materialList[3] on)
    on leftSave set val do (
		delegate.materialList[4].diffuseMap = val
		showTextureMap delegate.materialList[4] on)
    on frontSave set val do (
		delegate.materialList[5].diffuseMap = val
		showTextureMap delegate.materialList[5] on)	
    on backSave set val do (
		delegate.materialList[6].diffuseMap = val
		showTextureMap delegate.materialList[6] on)
  )
  rollout params "AWD Cube Material"
  (
	  
	Button  loadCubeMap "Load CubeMap from folder" width:310 pos:[10,10]
	Button  getUsersMat "Get Users" width:310  pos:[10,40]
	label lbl_Up "Up (pos Y)" width:80 pos:[10,73]
	mapButton  mbtn_up "map" width:230 pos:[90,70]
	label lbl_Down "Down (neg Y)" width:80 pos:[10,98]
	mapButton mbtn_down "map" width:230 pos:[90,95]
	label lbl_Right "Right (neg X)" width:80 pos:[10,123]
	mapButton mbtn_right "map" width:230 pos:[90,120]
	label lbl_Left "Left (pos X)" width:80 pos:[10,148]
	mapButton mbtn_left "map" width:230 pos:[90,145]
	label lbl_Front "Front (pos Z)" width:80 pos:[10,173]
	mapButton mbtn_front "map" width:230 pos:[90,170]
	label lbl_Back "Back (neg Z)" width:80 pos:[10,198]
	mapButton mbtn_back "map" width:230 pos:[90,195]
	label lbl_saveMode "Save textures:" width:80 pos:[10,233]
	dropdownlist ddl_saveMode1 ""  items:#("Use Globale Settings", "External File (Relative)", "Embed Bitmap") pos:[90,230] width:130 height:4 
	group "" (
		label lbl_space "" pos:[0,270] 
		checkBox cbx_ReplaceOnExport "Replace with ATFCubeTexture" pos:[15,275] enabled:false
		editText txt_ReplaceOnExportPath "Path" readOnly:true pos:[15,305] width:240 enabled:false
		button txt_BrowsePath "Browse" pos:[265,303] width:50 enabled:false
	)
	on getUsersMat pressed do(
		thisCubeTex=undefined
		--if material mode is "Compact-Mode"
		if MatEditor.mode ==#basic then (
			thisCubeTex = meditMaterials[activeMeditSlot] 
		)
		--if material mode is "Slate-Mode"
		else if MatEditor.mode ==#advanced then (
			thisCubeTex=sme.GetMtlInParamEditor()		
		)
		if thisCubeTex!=undefined then (
			getUsersForCubeTex thisCubeTex
		)
	)
	on loadCubeMap pressed do(
		thisCubeTex=undefined
		--if material mode is "Compact-Mode"
		if MatEditor.mode ==#basic then (
			thisCubeTex = meditMaterials[activeMeditSlot] 
		)
		--if material mode is "Slate-Mode"
		else if MatEditor.mode ==#advanced then (
			thisCubeTex=sme.GetMtlInParamEditor()		
		)
		if thisCubeTex!=undefined then loadCubeMapDirectory thisCubeTex
	)
	on cbx_ReplaceOnExport changed thisValue do (	
		if thisValue then (
			if saveReplaceURL=="" then (
				newfile = getOpenFileName  caption:"Select a texture file" initialDir:#images
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
  on create do
  (
	delegate.numsubs=6
    --delegate.opacityFalloff = 75
    -- setup initial material
    --delegate.opacityFalloff = 75
  )
)