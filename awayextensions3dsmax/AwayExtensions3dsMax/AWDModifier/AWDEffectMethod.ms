plugin modifier AWDEffectMethod
name:"AWDEffectMethod"
category:"AWDModifier"
classID:#(0x582d0573, 0x44755e63)
extends:EmptyModifier replaceUI:false version:1
(		
	local thisMesh
	local thisMod
	local params2
	local colorMatrixRollout
	local colorTranformRollout
	local EnvMapRollout			
	local LightMapRollout			
	local ProjectiveTextureRollout
	local RimLightRollout			
	local AlphaMaskRollout
	local RefractionEnvMapRollout
	local OutlineRollout
	local FresnelEnvMapRollout
	local FogRollout	
	
	fn getUsersForEffectMethod thisEffectMethod = (		
			global users=#()
			global userDispay=#()
			global userType=#()
			for mat in meditMaterials do(
				if classof mat==Standardmaterial then(
					hasSettings=undefined
					try hasSettings=mat.hasObjSettings
					catch (	hasSettings=undefined	)
					if hasSettings!=undefined then (
						for fxMethod in mat.saveMethods do(
							if fxMethod==thisEffectMethod then (
								isadded=appendIfUnique users mat
								if isadded then (
									append userDispay ("Material: '"+mat.name+"'")
									append userType "AWDMaterial"
								)		
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
									for fxMethod in submat.saveMethods do(
										if fxMethod==thisEffectMethod then (
											isadded=appendIfUnique users submat
											if isadded then (
												append userDispay ("Material: '"+submat.name+"'")
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
			for mat in scenematerials do(
				if classof mat==Standardmaterial then(
					hasSettings=undefined
					try hasSettings=mat.hasObjSettings
					catch (	hasSettings=undefined	)
					if hasSettings!=undefined then (
						for fxMethod in mat.saveMethods do(
							if fxMethod==thisEffectMethod then (
								isadded=appendIfUnique users mat
								if isadded then (
									append userDispay ("Material: '"+mat.name+"'")
									append userType "AWDMaterial"
								)		
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
									for fxMethod in submat.saveMethods do(
										if fxMethod==thisEffectMethod then (
											isadded=appendIfUnique users submat
											if isadded then (
												append userDispay ("Material: '"+submat.name+"'")
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
							for fxMethod in mat.saveMethods do(
								if fxMethod==thisEffectMethod then (
									isadded=appendIfUnique users mat
									if isadded then (
										append userDispay ("Material: '"+mat.name+"'")
										append userType "AWDMaterial"
									)		
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
										for fxMethod in submat.saveMethods do(
											if fxMethod==thisEffectMethod then (
												isadded=appendIfUnique users submat
												if isadded then (
													append userDispay ("Material: '"+submat.name+"'")
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
			)	

		if users.count>0 then(
			rollout select_users_rollout "AWDEffectMethod Users"	(
				listbox base_name "Users of selected AWDEffectMethod"  height:16 items:userDispay
				on base_name selected selected do (
					if userType[base_name.selection]=="AWDMaterial" then (
						if not MatEditor.IsOpen() then MatEditor.Open()
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
			messagebox("This AWDEffectMethod-modifier is not assigned to any material")
		)			
	)
	fn chooseAWDCubeMaterial thisObj forWhichElem= (				
		global awdCubeMaterialsDisplayNames=#()
		global awdCubeMaterials=#()	
		global thisMod2=thisObj
		global originalIdx=1
		global forWhichElement=forWhichElem
		foundAny=false
		cnt=1
		if forWhichElement==1 then thisReciever=thisMod2.envMap
		if forWhichElement==2 then thisReciever=thisMod2.refract_envMap
		if forWhichElement==3 then thisReciever=thisMod2.fresnel_EnvMap
		--find all CubeMaterials in the meditMaterials
		for mat in meditMaterials do(
			if (classof mat as string)=="AWDCubeMaterial" then(
				if thisReciever==mat then originalIdx=cnt
				cnt+=1
				appendThis= (appendIfUnique awdCubeMaterials mat)
				if appendThis then append awdCubeMaterialsDisplayNames (mat.name) 			
			)
		)
		--find all CubeMaterials in the sceneMaterials
		for mat in sceneMaterials do(
			if (classof mat as string)=="AWDCubeMaterial" then(
				if thisReciever==mat then originalIdx=cnt
				cnt+=1
				appendThis= (appendIfUnique awdCubeMaterials mat)
				if appendThis then append awdCubeMaterialsDisplayNames (mat.name) 			
			)
		)	
		--find all CubeMaterials in the views of the slate-material-editor
		numViews = sme.GetNumViews()
		for numView=1 to numViews do (
			oneView=sme.GetView numView
			numNodes = oneView.GetNumNodes()
			for numNode=1 to numNodes do(
				thisNode= trackViewNodes[#sme][numView][numNode].reference
				if (classof thisNode as string)=="AWDCubeMaterial" then(
					if thisReciever==thisNode then	originalIdx=cnt
					cnt+=1
					appendThis= (appendIfUnique awdCubeMaterials thisNode)
					if appendThis then append awdCubeMaterialsDisplayNames (thisNode.name) 			
				)
			)
		)		
		if awdCubeMaterials.count>0 then(
			rollout select_animSet_rollout "Select a AWDCubeMaterial"	(
				listbox base_name "AWDCubeMaterial available"  height:16 items:awdCubeMaterialsDisplayNames selection:originalIdx
				on base_name selected selected do (
					if (forWhichElement==1) then(
						thisMod2.envmapRollout.txt_cubeMatEnvMap.text = awdCubeMaterials[base_name.selection].name
						thisMod2.envMap=awdCubeMaterials[base_name.selection]
						thisMod2.envmapRollout.btn_selectEnvMap.enabled=true
						thisMod2.envmapRollout.btn_removeEnvMap.enabled=true
					)
					else if (forWhichElement==2) then (						
						thisMod2.refract_envMap=awdCubeMaterials[base_name.selection]
						thisMod2.RefractionEnvMapRollout.txt_refractionEnvMap.text=awdCubeMaterials[base_name.selection].name
						thisMod2.RefractionEnvMapRollout.btn_selectrefractionEnvMap.enabled=true
						thisMod2.RefractionEnvMapRollout.btn_removerefractionEnvMap.enabled=true
					)
					else if (forWhichElement==3) then (						
						thisMod2.fresnel_EnvMap=awdCubeMaterials[base_name.selection]
						thisMod2.FresnelEnvMapRollout.txt_fresnelEnvMap.text=awdCubeMaterials[base_name.selection].name
						thisMod2.FresnelEnvMapRollout.btn_selectfresnelEnvMap.enabled=true
						thisMod2.FresnelEnvMapRollout.btn_removefresnelEnvMap.enabled=true
					)
						
					DestroyDialog select_animSet_rollout
				)
			)
			createDialog select_animSet_rollout 300 240  modal:true
		)
		else	(
			messagebox("No AWDCubeMaterial could be found in the scene")
		)
	)		
	fn getAWDID = (
		newTime=getUniversalTime()
		newString="awd_"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+random 0.0 10000.0 as string
		return newString
	)
	
	fn showMods = (
							
		if params2.cbx_ColorMatrix.checked then addRollout colorMatrixRollout
		else removeRollout colorMatrixRollout
		if params2.cbx_ColorTransform.checked then addRollout colorTranformRollout
		else removeRollout colorTranformRollout
		if params2.cbx_EnvMap.checked then addRollout EnvMapRollout
		else removeRollout EnvMapRollout			
		if params2.cbx_LightMapMethod.checked then addRollout LightMapRollout
		else removeRollout LightMapRollout	
		if params2.cbx_ProjectiveTextureMethod.checked then addRollout ProjectiveTextureRollout
		else removeRollout ProjectiveTextureRollout
		if params2.cbx_RimLightMethod.checked then addRollout RimLightRollout
		else removeRollout RimLightRollout
		if params2.cbx_AlphaMaskMethod.checked then addRollout AlphaMaskRollout
		else removeRollout AlphaMaskRollout
		if params2.cbx_RefractionEnvMapMethod.checked then addRollout RefractionEnvMapRollout
		else removeRollout RefractionEnvMapRollout
		if params2.cbx_OutlineMethod.checked then addRollout OutlineRollout
		else removeRollout OutlineRollout
		if params2.cbx_FresnelEnvMapMethod.checked then addRollout FresnelEnvMapRollout
		else removeRollout FresnelEnvMapRollout
		if params2.cbx_FogMethod.checked then addRollout FogRollout
		else removeRollout FogRollout
	)
	parameters colorMatrixparams rollout:colorMatrixRollout
	(
		colorMatrixVal1 type:#float default:0 ui:spn_colorMatrixVal1 animatable:false
		colorMatrixVal2 type:#float default:0 ui:spn_colorMatrixVal2 animatable:false
		colorMatrixVal3 type:#float default:0 ui:spn_colorMatrixVal3 animatable:false
		colorMatrixVal4 type:#float default:1 ui:spn_colorMatrixVal4 animatable:false
		colorMatrixVal5 type:#float default:1 ui:spn_colorMatrixVal5 animatable:false
		colorMatrixVal6 type:#float default:0 ui:spn_colorMatrixVal6 animatable:false
		colorMatrixVal7 type:#float default:0 ui:spn_colorMatrixVal7 animatable:false
		colorMatrixVal8 type:#float default:0 ui:spn_colorMatrixVal8 animatable:false
		colorMatrixVal9 type:#float default:0 ui:spn_colorMatrixVal9 animatable:false
		colorMatrixVal10 type:#float default:1 ui:spn_colorMatrixVal10 animatable:false
		colorMatrixVal11 type:#float default:0 ui:spn_colorMatrixVal11 animatable:false
		colorMatrixVal12 type:#float default:0 ui:spn_colorMatrixVal12 animatable:false
		colorMatrixVal13 type:#float default:0 ui:spn_colorMatrixVal13 animatable:false
		colorMatrixVal14 type:#float default:0 ui:spn_colorMatrixVal14 animatable:false
		colorMatrixVal15 type:#float default:1 ui:spn_colorMatrixVal15 animatable:false
		colorMatrixVal16 type:#float default:0 ui:spn_colorMatrixVal16 animatable:false	
		colorMatrixVal17 type:#float default:0 ui:spn_colorMatrixVal17 animatable:false	
		colorMatrixVal18 type:#float default:0 ui:spn_colorMatrixVal18 animatable:false	
		colorMatrixVal19 type:#float default:0 ui:spn_colorMatrixVal19 animatable:false	
		colorMatrixVal20 type:#float default:1 ui:spn_colorMatrixVal20 animatable:false	
	)
	rollout colorMatrixRollout "Color Matrix" category:2
	(		
		spinner spn_colorMatrixVal1 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal2 ""  align:#left animatable:false
		spinner spn_colorMatrixVal3 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal4 ""  align:#left animatable:false
		spinner spn_colorMatrixVal5 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal6 ""  align:#left animatable:false
		spinner spn_colorMatrixVal7 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal8 ""  align:#left animatable:false
		spinner spn_colorMatrixVal9 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal10 ""  align:#left animatable:false
		spinner spn_colorMatrixVal11 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal12 ""   align:#left animatable:false
		spinner spn_colorMatrixVal13 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal14 ""  align:#left animatable:false
		spinner spn_colorMatrixVal15 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal16 ""  align:#left animatable:false
		spinner spn_colorMatrixVal17 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal18 ""  align:#left animatable:false
		spinner spn_colorMatrixVal19 "" across:2 align:#left animatable:false
		spinner spn_colorMatrixVal20 ""  align:#left animatable:false

	)
	parameters colorTransformparams rollout:colorTranformRollout
	(
		colorTransformColor type:#color default:[255,255,255] ui:spn_color1 animatable:false
		colorTransformVal1 type:#float default:1 ui:spn_colorTransformVal1 animatable:false
		colorTransformVal2 type:#float default:1 ui:spn_colorTransformVal2 animatable:false
		colorTransformVal3 type:#float default:1 ui:spn_colorTransformVal3 animatable:false
		colorTransformVal4 type:#float default:1 ui:spn_colorTransformVal4 animatable:false
		colorTransformVal5 type:#float default:1 ui:spn_colorTransformVal5 animatable:false
	)
	rollout colorTranformRollout "Color Transform" category:3
	(		
		colorPicker spn_color1 "Color" across:2 align:#left animatable:false
		spinner spn_colorTransformVal1 "Alpha" align:#left animatable:false
		spinner spn_colorTransformVal2 "R"  across:2 align:#left animatable:false
		spinner spn_colorTransformVal3 "G" align:#left animatable:false
		spinner spn_colorTransformVal4 "B"  across:2 align:#left animatable:false
		spinner spn_colorTransformVal5 "A" align:#left animatable:false
	)
	parameters envMapparams rollout:envmapRollout
	(
		envMapAlpha type:#float default:1.0 ui:spn_envMapAlpha animatable:false
		envMapMask type:#texturemap ui:set_Mask animatable:false
		envMap type:#maxObject  animatable:false
	)
	rollout envmapRollout "Env Map" category:4
	(		
		group "EnvMap (AWDCubeMaterial)"(
			edittext txt_cubeMatEnvMap "" readOnly:true animatable:false
			button btn_newEnvMap "new" width:30 across:4 animatable:false
			button btn_chooseEnvMap "add" width:30  animatable:false
			button btn_selectEnvMap "sel"  width:30  animatable:false
			button btn_removeEnvMap "del"  width:30  animatable:false
		)
		spinner spn_envMapAlpha "Alpha "  range:[0.0,1.0,0.001] align:#left animatable:false
		group "Mask (TexMap)"(
			mapButton set_Mask "Select Mask" width:130 animatable:false
			button btn_selectEnvMapMask "open"  width:60  across:2 animatable:false
			button btn_removeEnvMapMask "remove"  width:60  animatable:false
		)
		on  set_Mask picked  this do(
			btn_selectEnvMapMask.enabled=false
			btn_removeEnvMapMask.enabled=false
			if envMapMask!=undefined then (
				btn_selectEnvMapMask.enabled=true
				btn_removeEnvMapMask.enabled=true
			)
		)
		on btn_selectEnvMapMask pressed do(
			if not MatEditor.isOpen() then MatEditor.open() 
			if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = envMapMask
			else if MatEditor.mode ==#advanced then (
				numViews = sme.GetNumViews()
				isFound=false
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference								
						if thisNode == envMapMask then (
							sme.activeView=numView
							isFound=true
							exit									
						)
					)
					if isFound then exit
				)
				if not isFound then (
					cleanNameBool=false
					cleanNameCnt =  1
					cleanName = "EnvMap"
					mainName = "EnvMap"
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
					thisView.CreateNode envMapMask [0,0]					
				)
			)
		)
		on btn_removeEnvMapMask pressed do(
			envMapMask=undefined
			btn_selectEnvMapMask.enabled=false
			btn_removeEnvMapMask.enabled=false
		)
		on btn_newEnvMap pressed do(
			theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDCubeMaterial")
			theobj.text ="Add name for new AWDCubeMaterial"
			DialogResult = theObj.Showmodal()
			dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
			result = theobj.InstanceName
			newCubeMat=AWDCubeMaterial()
			newCubeMat.name = result
			envMap=newCubeMat
			txt_cubeMatEnvMap.text=envMap.name
			btn_selectEnvMap.enabled=true
			btn_removeEnvMap.enabled=true
		)
		on btn_chooseEnvMap pressed do(
			curMod=modPanel.GetCurrentObject()
			chooseAWDCubeMaterial curMod 1
		)
		on btn_selectEnvMap pressed do(
			if not MatEditor.isOpen() then MatEditor.open() 
			if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = envMap
			else if MatEditor.mode ==#advanced then (
				numViews = sme.GetNumViews()
				isFound=false
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference								
						if thisNode == envMap then (
							sme.activeView=numView
							isFound=true
							exit									
						)
					)
					if isFound then exit
				)
				if not isFound then (
					cleanNameBool=false
					cleanNameCnt =  1
					cleanName = "AWDCubeTexture"
					mainName = "AWDCubeTexture"
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
					thisView.CreateNode envMap [0,0]					
				)
			)
		)
		on btn_removeEnvMap pressed do(
			txt_cubeMatEnvMap.text=""
			envMap=undefined
			btn_selectEnvMap.enabled=false
			btn_removeEnvMap.enabled=false
		)
		on envmapRollout open do (
			btn_selectEnvMap.enabled=false
			btn_removeEnvMap.enabled=false
			if envMap!=undefined then (
				txt_cubeMatEnvMap.text=envMap.name
				btn_selectEnvMap.enabled=true
				btn_removeEnvMap.enabled=true
			)
			btn_selectEnvMapMask.enabled=false
			btn_removeEnvMapMask.enabled=false
			if envMapMask!=undefined then (
				btn_selectEnvMapMask.enabled=true
				btn_removeEnvMapMask.enabled=true
			)
		)
	)
	parameters LightMapparams rollout:LightMapRollout
	(
		useSecUV type:#Boolean default:false ui:cbx_useSecUV  animatable:false 
		blendMode type:#integer default:1 ui:ddl_blendMode animatable:false 
		thislightMap type:#texturemap ui:set_lightMap animatable:false
	)
	rollout LightMapRollout "Light Map" category:5
	(	
		group "Light Map (TexMap)"(
			mapButton set_lightMap "Select LightMap" width:130 animatable:false
			button btn_selectLightMap "open"  width:60  across:2 animatable:false
			button btn_removeLightMap "remove"  width:60  animatable:false
		)
		on btn_selectLightMap pressed do(
			if not MatEditor.isOpen() then MatEditor.open() 
			if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = thislightMap
			else if MatEditor.mode ==#advanced then (
				numViews = sme.GetNumViews()
				isFound=false
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference								
						if thisNode == thislightMap then (
							sme.activeView=numView
							isFound=true
							exit									
						)
					)
					if isFound then exit
				)
				if not isFound then (
					cleanNameBool=false
					cleanNameCnt =  1
					cleanName = "LightMap"
					mainName = "LightMap"
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
					thisView.CreateNode thislightMap [0,0]					
				)
			)
		)
		on btn_removeLightMap pressed do(
			thislightMap=undefined
			btn_selectLightMap.enabled=false
			btn_removeLightMap.enabled=false
		)
		on  set_lightMap picked  this do(
			btn_selectLightMap.enabled=false
			btn_removeLightMap.enabled=false
			if thislightMap!=undefined then (
				btn_selectLightMap.enabled=true
				btn_removeLightMap.enabled=true
			)
		)
		checkBox cbx_useSecUV "Use Secondary UV"  align:#left animatable:false
		dropdownlist ddl_blendMode "BlendMode"  items:#("MULTIPLY", "ADD")  height:3  align:#left animatable:false
		on LightMapRollout open do (
			btn_selectLightMap.enabled=false
			btn_removeLightMap.enabled=false
			if thislightMap!=undefined then (
				btn_selectLightMap.enabled=true
				btn_removeLightMap.enabled=true
			)
		)
	)
	parameters ProjectiveTextureparams rollout:ProjectiveTextureRollout
	(
	)
	rollout ProjectiveTextureRollout "Projective Texture" category:6
	(		
	)
	parameters RimLightparams rollout:RimLightRollout
	(
		thisColor type:#color default:[255,255,255] ui:cpk_thisColor animatable:false
		thisStrength type:#float default:0.4 ui:spn_thisStrength animatable:false
		thispower type:#float default:2 ui:spn_tthispower animatable:false
	)
	rollout RimLightRollout "Rim Light" category:7
	(		
		colorpicker cpk_thisColor "Color"  align:#left animatable:false
		spinner spn_thisStrength "Strength" range:[0.0,1.0,0.01] align:#left animatable:false
		spinner spn_tthispower "Power"  range:[0.0,10.0,0.1]  align:#left animatable:false
	)
	parameters AlphaMaskparams rollout:AlphaMaskRollout 
	(
		mask_useSecUV type:#Boolean default:false ui:cbx_mask_useSecUV animatable:false
		mask_map type:#texturemap ui:set_Mask animatable:false
	)
	rollout AlphaMaskRollout "Alpha Mask" category:8
	(		
		group "Alpha Mask (TexMap)"(
			mapButton set_Mask "Select LightMap" width:130 animatable:false
			button btn_selectAlphaMask "open"  width:60  across:2 animatable:false
			button btn_removeAlphaMask "remove"  width:60  animatable:false
		)
		on btn_selectAlphaMask pressed do(
			if not MatEditor.isOpen() then MatEditor.open() 
			if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = mask_map
			else if MatEditor.mode ==#advanced then (
				numViews = sme.GetNumViews()
				isFound=false
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference								
						if thisNode == mask_map then (
							sme.activeView=numView
							isFound=true
							exit									
						)
					)
					if isFound then exit
				)
				if not isFound then (
					cleanNameBool=false
					cleanNameCnt =  1
					cleanName = "AlphaMask"
					mainName = "AlphaMask"
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
					thisView.CreateNode mask_map [0,0]					
				)
			)
		)
		on btn_removeAlphaMask pressed do(
			mask_map=undefined
			btn_selectAlphaMask.enabled=false
			btn_removeAlphaMask.enabled=false
		)
		on  set_Mask picked  this do(
			btn_selectAlphaMask.enabled=false
			btn_removeAlphaMask.enabled=false
			if mask_map!=undefined then (
				btn_selectAlphaMask.enabled=true
				btn_removeAlphaMask.enabled=true
			)
		)
		checkBox cbx_mask_useSecUV "Use Secondary UV"  align:#left animatable:false
		on AlphaMaskRollout open do(
			btn_selectAlphaMask.enabled=false
			btn_removeAlphaMask.enabled=false
			if mask_map!=undefined then (
				btn_selectAlphaMask.enabled=true
				btn_removeAlphaMask.enabled=true
			)
		)
	)
	parameters RefractionEnvMapparams rollout:RefractionEnvMapRollout
	(
		refract_envMap type:#maxObject animatable:false
		refract_dispersion1 type:#float default:0.01 ui:spn_refract_dispersion1 animatable:false
		refract_dispersion2 type:#float default:0.01 ui:spn_refract_dispersion2 animatable:false
		refract_dispersion3 type:#float default:0.01 ui:spn_refract_dispersion3 animatable:false
		refract_index type:#float default:0.1 ui:spn_refract_index animatable:false
		refract_alpha type:#float default:1 ui:spn_refract_alpha animatable:false
	)
	rollout RefractionEnvMapRollout "Refraction Env Map" category:9
	(		
		group "EnvMap (AWDCubeMaterial)"(
			edittext txt_refractionEnvMap "" readOnly:true animatable:false
			button btn_newrefractionEnvMap "new" width:30 across:4 animatable:false
			button btn_chooserefractionEnvMap "add" width:30  animatable:false
			button btn_selectrefractionEnvMap "sel"  width:30  animatable:false
			button btn_removerefractionEnvMap "del"  width:30  animatable:false
		)
		spinner spn_refract_dispersion1 "Dispersion R"  align:#left animatable:false
		spinner spn_refract_dispersion2 "Dispersion G"  align:#left animatable:false
		spinner spn_refract_dispersion3 "Dispersion B" align:#left animatable:false
		spinner spn_refract_index "Refraction idx" align:#left animatable:false
		spinner spn_refract_alpha "Alpha" align:#left animatable:false
		on btn_newrefractionEnvMap pressed do(
			theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDCubeMaterial")
			theobj.text ="Add name for new AWDCubeMaterial"
			DialogResult = theObj.Showmodal()
			dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
			result = theobj.InstanceName
			newCubeMat=AWDCubeMaterial()
			newCubeMat.name = result
			refract_envMap=newCubeMat
			txt_refractionEnvMap.text=refract_envMap.name
			btn_selectrefractionEnvMap.enabled=true
			btn_removerefractionEnvMap.enabled=true
		)
		on btn_chooserefractionEnvMap pressed do(
			curMod=modPanel.GetCurrentObject()
			chooseAWDCubeMaterial curMod 2
		)
		on btn_selectrefractionEnvMap pressed do(
			if not MatEditor.isOpen() then MatEditor.open() 
			if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = refract_envMap
			else if MatEditor.mode ==#advanced then (
				numViews = sme.GetNumViews()
				isFound=false
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference								
						if thisNode == refract_envMap then (
							sme.activeView=numView
							isFound=true
							exit									
						)
					)
					if isFound then exit
				)
				if not isFound then (
					cleanNameBool=false
					cleanNameCnt =  1
					cleanName = "AWDCubeTexture"
					mainName = "AWDCubeTexture"
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
					thisView.CreateNode refract_envMap [0,0]					
				)
			)
		)
		on btn_removerefractionEnvMap pressed do(
			txt_refractionEnvMap.text=""
			refract_envMap=undefined
			btn_selectrefractionEnvMap.enabled=false
			btn_removerefractionEnvMap.enabled=false
		)
		on RefractionEnvMapRollout open do (
			btn_selectrefractionEnvMap.enabled=false
			btn_removerefractionEnvMap.enabled=false
			if refract_envMap!=undefined then (
				txt_refractionEnvMap.text=refract_envMap.name
				btn_selectrefractionEnvMap.enabled=true
				btn_removerefractionEnvMap.enabled=true
			)
		)
		
		
	)
	parameters Outlineparams rollout:OutlineRollout
	(
		outlineColor type:#color default:[0,0,0] ui:cpk_outlineColor animatable:false
		outlineSize type:#float default:1 ui:spn_outlineSize animatable:false
		innerLines type:#Boolean default:true ui:cbx_innerLines animatable:false
		dedicadedMesh type:#Boolean default:false ui:cbx_dedicadedMesh animatable:false
	)
	rollout OutlineRollout "Outline" category:10
	(		
		colorpicker cpk_outlineColor "Outline Color" align:#left animatable:false
		spinner spn_outlineSize "Outline-Size" align:#left animatable:false
		checkBox cbx_innerLines "Show Inner Lines" align:#left animatable:false
		checkBox cbx_dedicadedMesh "Dedicated Mesh" align:#left animatable:false
	)
	parameters FresnelEnvMapparams rollout:FresnelEnvMapRollout
	(
		fresnel_EnvMap  type:#maxObject animatable:false
		fresnel_alpha type:#float default:1.0 ui:spn_fresnel_alpha animatable:false
	)
	rollout FresnelEnvMapRollout "Fresnel Env Map" category:11
	(		
		group "EnvMap (AWDCubeMaterial)"(
			edittext txt_fresnelEnvMap "" readOnly:true animatable:false
			button btn_newfresnelEnvMap "new" width:30 across:4 animatable:false
			button btn_choosefresnelEnvMap "add" width:30  animatable:false
			button btn_selectfresnelEnvMap "sel"  width:30  animatable:false
			button btn_removefresnelEnvMap "del"  width:30  animatable:false
		)
		spinner spn_fresnel_alpha "Alpha" range:[0.0,1.0,0.1]  align:#left animatable:false
		on btn_newfresnelEnvMap pressed do(
			theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDCubeMaterial")
			theobj.text ="Add name for new AWDCubeMaterial"
			DialogResult = theObj.Showmodal()
			dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
			result = theobj.InstanceName
			newCubeMat=AWDCubeMaterial()
			newCubeMat.name = result
			fresnel_EnvMap=newCubeMat
			txt_fresnelEnvMap.text=fresnel_EnvMap.name
			btn_selectfresnelEnvMap.enabled=true
			btn_removefresnelEnvMap.enabled=true
		)
		on btn_choosefresnelEnvMap pressed do(
			curMod=modPanel.GetCurrentObject()
			chooseAWDCubeMaterial curMod 3
		)
		on btn_selectfresnelEnvMap pressed do(
			if not MatEditor.isOpen() then MatEditor.open() 
			if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = fresnel_EnvMap
			else if MatEditor.mode ==#advanced then (
				numViews = sme.GetNumViews()
				isFound=false
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference								
						if thisNode == fresnel_EnvMap then (
							sme.activeView=numView
							isFound=true
							exit									
						)
					)
					if isFound then exit
				)
				if not isFound then (
					cleanNameBool=false
					cleanNameCnt =  1
					cleanName = "FresnelEnvMap"
					mainName = "FresnelEnvMap"
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
					thisView.CreateNode fresnel_EnvMap [0,0]					
				)
			)
		)
		on btn_removefresnelEnvMap pressed do(
			txt_fresnelEnvMap.text=""
			fresnel_EnvMap=undefined
			btn_selectfresnelEnvMap.enabled=false
			btn_removefresnelEnvMap.enabled=false
		)
		on FresnelEnvMapRollout open do (
			btn_selectfresnelEnvMap.enabled=false
			btn_removefresnelEnvMap.enabled=false
			if fresnel_EnvMap!=undefined then (
				txt_fresnelEnvMap.text=fresnel_EnvMap.name
				btn_selectfresnelEnvMap.enabled=true
				btn_removefresnelEnvMap.enabled=true
			)
		)
	)
	parameters Fogparams rollout:FogRollout 
	(
		fogDistanceMin type:#float default:1 ui:spn_fogMin animatable:false
		fogDistanceMax type:#float default:1000.0 ui:spn_fogMax animatable:false
		fogColor type:#color default:[200,200,200] ui:col_fogColor animatable:false
	)
	rollout FogRollout "Fog" category:12
	(		
		colorpicker col_fogColor "Fog Color" align:#left animatable:false
		spinner spn_fogMin "Min Distance" range:[1.0,9999999.0,1.0] align:#left animatable:false
		spinner spn_fogMax "Max Distance"  range:[1.0,9999999.0,1.0] align:#left animatable:false
	)
	parameters main2 rollout:params2
	(
		thisAWDID type:#string default:"noIDSet" animatable:false
		isColorMatrix type:#boolean default:false ui:cbx_ColorMatrix animatable:false
		isColorTransform type:#boolean default:false ui:cbx_ColorTransform animatable:false
		isEnvMap type:#boolean default:false ui:cbx_EnvMap animatable:false
		isLightMapMethod type:#boolean default:false ui:cbx_LightMapMethod animatable:false
		isProjectiveTextureMethod type:#boolean default:false ui:cbx_ProjectiveTextureMethod animatable:false
		isRimLightMethod type:#boolean default:false ui:cbx_RimLightMethod animatable:false
		isAlphaMaskMethod type:#boolean default:false ui:cbx_AlphaMaskMethod animatable:false
		isRefractionEnvMapMethod type:#boolean default:false ui:cbx_RefractionEnvMapMethod animatable:false
		isOutlineMethod type:#boolean default:false ui:cbx_OutlineMethod animatable:false
		isFresnelEnvMapMethod type:#boolean default:false ui:cbx_FresnelEnvMapMethod animatable:false
		isFogMethod type:#boolean default:false ui:cbx_FogMethod animatable:false
		
		idxColorMatrix type:#integer default:1 ui:spn_idxColorMatrix animatable:false
		idxColorTransform type:#integer default:2 ui:spn_idxColorTransform animatable:false
		idxEnvMap type:#integer default:3 ui:spn_idxEnvMap animatable:false
		idxLightMapMethod type:#integer default:4 ui:spn_idxLightMapMethod animatable:false
		idxProjectiveTextureMethod type:#integer default:5 ui:spn_idxProjectiveTextureMethod animatable:false
		idxRimLightMethod type:#integer default:6 ui:spn_idxRimLightMethod animatable:false
		idxAlphaMaskMethod type:#integer default:7 ui:spn_idxAlphaMaskMethod animatable:false
		idxRefractionEnvMapMethod type:#integer default:8 ui:spn_idxRefractionEnvMapMethod animatable:false
		idxOutlineMethod type:#integer default:9 ui:spn_idxOutlineMethod animatable:false
		idxFresnelEnvMapMethod type:#integer default:10 ui:spn_idxFresnelEnvMapMethod animatable:false
		idxFogMethod type:#integer default:11 ui:spn_idxFogMethod animatable:false
	)
	rollout params2 "AWD Effect Method"  category:1
	(	
		button btn_findusers "Get users"  animatable:false
		checkBox cbx_ColorMatrix "ColorMatrix" animatable:false across:2
		spinner spn_idxColorMatrix "" type:#integer range:[0,100,1] animatable:false
		checkBox cbx_ColorTransform "ColorTransform" animatable:false across:2
		spinner spn_idxColorTransform "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_EnvMap "EnvMap" animatable:false across:2
		spinner spn_idxEnvMap "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_LightMapMethod "LightMap" animatable:false across:2
		spinner spn_idxLightMapMethod "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_ProjectiveTextureMethod "ProjectiveTex" enabled:false animatable:false across:2
		spinner spn_idxProjectiveTextureMethod "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_RimLightMethod "RimLight" animatable:false across:2
		spinner spn_idxRimLightMethod "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_AlphaMaskMethod "AlphaMask" animatable:false across:2
		spinner spn_idxAlphaMaskMethod "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_RefractionEnvMapMethod "RefractionEnv" animatable:false across:2
		spinner spn_idxRefractionEnvMapMethod "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_OutlineMethod "Outline" animatable:false across:2
		spinner spn_idxOutlineMethod "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_FresnelEnvMapMethod "FresnelEnvMap" animatable:false across:2
		spinner spn_idxFresnelEnvMapMethod "" type:#integer range:[0,100,1]  animatable:false
		checkBox cbx_FogMethod "Fog" animatable:false across:2
		spinner spn_idxFogMethod "" type:#integer range:[0,100,1]  animatable:false
		on btn_findusers pressed do (
			curMod=modPanel.GetCurrentObject()
			getUsersForEffectMethod curMod
		)
		on  cbx_ColorMatrix changed state do(
			showMods()
		)
		on  cbx_ColorTransform changed state do(
			showMods()
		)
		on  cbx_EnvMap changed state do(
			showMods()
		)
		on  cbx_LightMapMethod changed state do(
			showMods()
		)
		on  cbx_ProjectiveTextureMethod changed state do(
			showMods()
		)
		on  cbx_RimLightMethod changed state do(
			showMods()
		)
		on  cbx_AlphaMaskMethod changed state do(
			showMods()
		)
		on  cbx_RefractionEnvMapMethod changed state do(
			showMods()
		)
		on  cbx_OutlineMethod changed state do(
			showMods()
		)
		on  cbx_FresnelEnvMapMethod changed state do(
			showMods()
		)
		on  cbx_FogMethod changed state do(
			showMods()
		)
		on params2 rolledUp state do(
			showMods()
		)
		on params2 help do messagebox("no help available yet")
		on params2 open do(
			showMods()
		)
	)

	on create do (
		--curAnimSetMod=modPanel.GetCurrentObject()
		--delegate.name = uniquename "AWD Anim Set"
		if thisAWDID=="noIDSet" then(
			thisAWDID=getAWDID()
		)
	)
	on clone orig do (				
		thisAWDID=getAWDID()		
	)

)