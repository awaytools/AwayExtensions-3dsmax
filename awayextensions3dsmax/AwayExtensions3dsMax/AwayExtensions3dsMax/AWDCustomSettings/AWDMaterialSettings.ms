
fn chooseAWDCubeMaterial thisObj forWhichElem= (				
	global awdCubeMaterialsDisplayNames=#()
	global awdCubeMaterials=#()	
	global thisMod2=thisObj
	global originalIdx=1
	global forWhichElement=forWhichElem
	foundAny=false
	cnt=1
	if forWhichElement==1 then thisReciever=thisMod2.ambient_envMap
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
					thisMod2.EnvMapAmbientRollUp.txt_ambientenvMap.text = awdCubeMaterials[base_name.selection].name
					thisMod2.ambient_envMap=awdCubeMaterials[base_name.selection])
					thisMod2.EnvMapAmbientRollUp.btn_selectambientenvMap.enabled=true
					thisMod2.EnvMapAmbientRollUp.btn_removeambientenvMap.enabled=true
					
				DestroyDialog select_animSet_rollout
			)
		)
		createDialog select_animSet_rollout 300 240  modal:true
	)
	else	(
		messagebox("No AWDCubeMaterial could be found in the scene")
	)
)	

--choose a AWD-effect-method that is available in the scene
fn chooseShadow thisMtl = (	
	--global to allow to access vars from dialog-rollout.	
	global availableModsDisplayNames=#()
	global availableModsList=#()	
	global thisMat=thisMtl
	foundAny=false
	global selectionIdx=1
	selectionIdxCnt=1
	for i in objects do (
		for m in i.modifiers do(
			if (classof m as string)=="AWDShadowMethod" then(
				if  m==thisMat.shadowMod then(
					selectionIdx=selectionIdxCnt
				)
				selectionIdxCnt+=1
				foundAny=true
				appendThis= (appendIfUnique availableModsList m)
				if appendThis then(
					append availableModsDisplayNames ("'"+m.name+"'  object: "+i.name) 
				)
			)
		)
	)
	if availableModsList.count>0 then (
		rollout select_mesh_rollout "Select a AWDLight-Modifier"	(
			listbox base_name "AWDLight-Modifier available in scene"  height:16 items:availableModsDisplayNames selection:selectionIdx
			on base_name selected selected do (
				
				thisMat.AWD_MaterialSettings.btn_selectshadowMod.enabled=true
				thisMat.AWD_MaterialSettings.btn_removeshadowMod.enabled=true
				thisMat.AWD_MaterialSettings.txt_shadowMod.text=availableModsList[base_name.selection].name
				thisMat.shadowMod = availableModsList[base_name.selection]
				thisMat.shadowModID = availableModsList[base_name.selection].thisAWDID
				DestroyDialog select_mesh_rollout
			)
		)
		--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
		createDialog select_mesh_rollout 300 240  modal:true
	)
	else	(
		if foundAny then	messagebox("All available AWDLight-modifier are allready assigned to this Material.")
		else messagebox("No AWDLight-modifier found in the scene")
	)
)
	
--choose a AWD-effect-method that is available in the scene
fn chooseEffectMethod thisMtl = (	
	--global to allow to access vars from dialog-rollout.	
	global availableModsDisplayNames=#()
	global availableModsList=#()	
	global thisMat=thisMtl
	foundAny=false
	for i in objects do (
		for m in i.modifiers do(
			if (classof m as string)=="AWDEffectMethod" then(
				foundThis=false
				for one in thisMat.saveMethods do (
					if m==thisMat.saveMethods then (
						foundThis=true
					)
				)
				if not foundThis then (
					foundAny=true
					appendThis= (appendIfUnique availableModsList m)
					if appendThis then(
						append availableModsDisplayNames ("'"+m.name+"'  object: "+i.name) 
					)
				)
			)
		)
	)
	if availableModsList.count>0 then (
		rollout select_mesh_rollout "Select a AWDEffectMethod-Modifier"	(
			listbox base_name "AWDEffectMethod-Modifier available in scene"  height:16 items:availableModsDisplayNames selection:1
			on base_name selected selected do (
				append thisMat.saveMethods availableModsList[base_name.selection]
				append thisMat.saveMethodIds availableModsList[base_name.selection].thisAWDID
				
				thisMat.AWD_EffectMethods.method_list.items = append thisMat.AWD_EffectMethods.method_list.items availableModsList[base_name.selection].name
				thisMat.AWD_EffectMethods.btn_select_effectMethod.enabled=true
				thisMat.AWD_EffectMethods.btn_clear_effectMethod.enabled=true
				thisMat.AWD_EffectMethods.method_list.selection=thisMat.AWD_EffectMethods.method_list.items.count
				DestroyDialog select_mesh_rollout
			)
		)
		--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
		createDialog select_mesh_rollout 300 240  modal:true
	)
	else	(
		if foundAny then	messagebox("All available AWDEffectMethod-modifier are allready assigned to this Material.")
		else messagebox("No AWDEffectMethod-modifier found in the scene")
	)
)
	
--Sets the visibility of the activ shading method rollups
fn showShadingrollups mat = (	
	removeRollout mat.EnvMapAmbientRollUp
	removeRollout mat.GradientDiffuseRollUp
	removeRollout mat.WarpDiffuseRollUp
	removeRollout mat.LightMapDiffuseRollUp
	removeRollout mat.CellDiffuseRollUp
	removeRollout mat.CellSpecularRollUp
	removeRollout mat.FresnelSpecularRollUp
	removeRollout mat.SimpleWaterNormalRollUp
	removeRollout mat.AWD_EffectMethods
	--mat.AnimClipsRollout.open=false
	--mat.AWD_EffectMethods.open=false
	
	if not mat.shadingenabled then removeRollout mat.AWDShadingRollout	
	--removeRollout mat.AWD_EffectMethods	
	--removeRollout mat.AWD_MaterialSettings
	if mat.methodenabled then addRollout mat.AWD_EffectMethods ----rolledUp:true
		
	if mat.shadingenabled then (
		if mat.AWDShadingRollout.ddl_ambient.selection==2 then addRollout mat.EnvMapAmbientRollUp --rolledUp:true
		if mat.AWDShadingRollout.ddl_diffuse.selection==2 then addRollout mat.GradientDiffuseRollUp --rolledUp:true
		if mat.AWDShadingRollout.ddl_diffuse.selection==3 then addRollout mat.WarpDiffuseRollUp --rolledUp:true
		if mat.AWDShadingRollout.ddl_diffuse.selection==4 then (
			addRollout mat.LightMapDiffuseRollUp --rolledUp:true
			if mat.LightMapDiffuseRollUp.ddl_lightMapbase.selection==2 then addRollout mat.GradientDiffuseRollUp --rolledUp:true
			if mat.LightMapDiffuseRollUp.ddl_lightMapbase.selection==3 then addRollout mat.WarpDiffuseRollUp --rolledUp:true
		)
		if mat.AWDShadingRollout.ddl_diffuse.selection==5 then (
			addRollout mat.CellDiffuseRollUp --rolledUp:true
			if mat.CellDiffuseRollUp.ddl_cellDiffusebase.selection==2 then addRollout mat.GradientDiffuseRollUp --rolledUp:true
			if mat.CellDiffuseRollUp.ddl_cellDiffusebase.selection==3 then addRollout mat.WarpDiffuseRollUp --rolledUp:true
		)
		if mat.AWDShadingRollout.ddl_spec.selection==4 then addRollout mat.CellSpecularRollUp --rolledUp:true
		if mat.AWDShadingRollout.ddl_spec.selection==5 then addRollout mat.FresnelSpecularRollUp --rolledUp:true
		if mat.AWDShadingRollout.ddl_normal.selection==2 then addRollout mat.SimpleWaterNormalRollUp --rolledUp:true
		addRollout mat.AWDShadingRollout-- rolledUp:false
	)
)

-- add AWDMaterialSetting to the given material
fn addAWDMaterialSettings thisMat = (
	if classof thisMat != Standardmaterial then return false	
	hasSettings=undefined
	try hasSettings=thisMat.hasObjSettings
	catch (	hasSettings=undefined	)
	if hasSettings!=undefined then (
		return false
	)
	else (
		AWDAttributesCA = Attributes "AWDMaterialSettings"
		(
			local AWD_EffectMethods
			parameters EnvMapAmbientParams rollout:EnvMapAmbientRollUp
			(		
				ambient_envMap type:#maxObject animateable:false
			)
			rollout EnvMapAmbientRollUp "EnvMap Ambient" category:10
			(		
				group "EnvMap (AWDCubeMaterial)"(
					edittext txt_ambientenvMap "" readOnly:true animatable:false
					button btn_newambientenvMap "new" width:60 across:4 animatable:false
					button btn_chooseambientenvMap "choose" width:60  animatable:false
					button btn_selectambientenvMap "open"  width:60  animatable:false
					button btn_removeambientenvMap "remove"  width:60  animatable:false
				)				
				on btn_newambientenvMap pressed do(global AWDCubeMaterial
					
					theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDCubeMaterial")
					theobj.text ="Add name for new AWDCubeMaterial"
					DialogResult = theObj.Showmodal()
					dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
					result = theobj.InstanceName
					newCubeMat=AWDCubeMaterial()
					newCubeMat.name = result
					ambient_envMap=newCubeMat
					txt_ambientenvMap.text=ambient_envMap.name
					btn_selectambientenvMap.enabled=true
					btn_removeambientenvMap.enabled=true
				)
				on btn_chooseambientenvMap pressed do(
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						chooseAWDCubeMaterial mtl 1
					)					
				)
				on btn_selectambientenvMap pressed do(
					if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = ambient_envMap
					else if MatEditor.mode ==#advanced then (
						numViews = sme.GetNumViews()
						isFound=false
						for numView=1 to numViews do (
							oneView=sme.GetView numView
							numNodes = oneView.GetNumNodes()
							for numNode=1 to numNodes do(
								thisNode= trackViewNodes[#sme][numView][numNode].reference								
								if thisNode == ambient_envMap then (
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
							thisView.CreateNode ambient_envMap [0,0]
							
						)
					)
				)
				on btn_removeambientenvMap pressed do(
					txt_ambientenvMap.text=""
					ambient_envMap=undefined
					btn_selectambientenvMap.enabled=false
					btn_removeambientenvMap.enabled=false
				)
				on EnvMapAmbientRollUp open do (
					btn_selectambientenvMap.enabled=false
					btn_removeambientenvMap.enabled=false
					if ambient_envMap!=undefined then (
						txt_ambientenvMap.text=ambient_envMap.name
						btn_selectambientenvMap.enabled=true
						btn_removeambientenvMap.enabled=true
					)
				)
			)
			parameters GradientDiffuseParams rollout:GradientDiffuseRollUp
			(		
				gradientdiffuse_map type:#texturemap ui:mb_gradientMap animateable:false
			)
			rollout GradientDiffuseRollUp "Gradient Diffuse" category:11
			(		
				group "Gradient Map (TexMap)"(
					mapButton mb_gradientMap "Select Gradient Map" width:270 animatable:false
				)
			)
			parameters WarpDiffuseParams rollout:WarpDiffuseRollUp
			(		
				warpdiffuse_factor type:#float default:5 ui:spn_warpdiffuse_factor animateable:false
			)
			rollout WarpDiffuseRollUp "Wrap Diffuse" category:12
			(		
				Spinner spn_warpdiffuse_factor "Wrap Factor"	animateable:false	
			)
			parameters LightMapDiffuseParams rollout:LightMapDiffuseRollUp
			(		
				lightMap_baseMethod type:#integer default:1 ui:ddl_lightMapbase animateable:false
				light_map type:#texturemap ui:mb_lightMap animateable:false
				light_map_blendMode type:#integer default:1 ui:ddl_lightMapblendMode animatable:false
			)
			rollout LightMapDiffuseRollUp "Lightmap Diffuse" category:13
			(		
				dropDownList ddl_lightMapbase "Base Method" items:#("BasicDiffuse", "GradientDiffuse", "WrapDiffuse") animateable:false
				dropdownlist ddl_lightMapblendMode "BlendMode"  items:#("NORMAL", "LAYER", "MULTIPLY", "ADD" , "ALPHA" )  height:6  align:#left animatable:false
				on ddl_lightMapbase selected  selectedIdx do (
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)	
				group " Light Map (TexMap)"(
					mapButton mb_lightMap "Select Light Map" width:270 animatable:false
				)
			)
			parameters CellDiffuseParams rollout:CellDiffuseRollUp
			(		
				celldiffuse_level type:#integer default:3 ui:spn_celldiffuse_level animateable:false
				celldiffuse_smooth type:#float default:0.1 ui:spn_celldiffuse_smooth animateable:false
				celldiffuse_baseMethod type:#integer default:1 ui:ddl_cellDiffusebase animateable:false
			)
			rollout CellDiffuseRollUp "Cell Diffuse" category:14
			(		
				Spinner spn_celldiffuse_level "Level" type:#integer animateable:false		
				Spinner spn_celldiffuse_smooth "Smoothness" animateable:false		
				dropDownList ddl_cellDiffusebase "Base Method" items:#("BasicDiffuse", "GradientDiffuse", "WarpDiffuse") animateable:false
				on ddl_cellDiffusebase selected  selectedIdx do (
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)
			)
			parameters CellSpecularParams rollout:CellSpecularRollUp
			(		
				cellspec_cutoff type:#float default:0.5 ui:spn_cellspec_cutoff animateable:false
				cellspec_smooth type:#float default:0.1 ui:spn_cellspec_smooth animateable:false
				cellspec_baseMethod type:#integer default:1 ui:ddl_cellspecbase animateable:false
			)
			rollout CellSpecularRollUp "Cell Specular" category:15
			(		
				Spinner spn_cellspec_cutoff "Cutoff" animateable:false		
				Spinner spn_cellspec_smooth "Smoothness"	animateable:false	
				dropDownList ddl_cellspecbase "Base Method" items:#("BasicSpecular", "AnisotropicSpecular", "Phong") animateable:false
			)
			
			parameters FresnelSpecularParams rollout:FresnelSpecularRollUp
			(		
				fresnelspec_basedOnSurface type:#Boolean default:true ui:cbx_fresnelspec_basedOnSurface animateable:false
				fresnelspec_power type:#integer default:5 ui:spn_cellspec_power animateable:false
				fresnelspec_reflect type:#float default:0.1 ui:spn_cellspec_reflect animateable:false
				fresnelspec_baseMethod type:#integer default:1 ui:ddl_fresnelspecbase animateable:false
			)
			rollout FresnelSpecularRollUp "Fresnell Specular" category:16
			(		
				checkBox cbx_fresnelspec_basedOnSurface "Based on Surface" animateable:false
				Spinner spn_cellspec_power "Power" animateable:false		
				Spinner spn_cellspec_reflect "Normal Reflect" animateable:false		 
				dropDownList ddl_fresnelspecbase "Base Method" items:#("BasicSpecular", "AnisotropicSpecular", "Phong") animateable:false	
			)
			parameters SimpleWaterNormalParams rollout:SimpleWaterNormalRollUp
			(		
				waternormal_map type:#texturemap ui:mb_waternormal_map animateable:false
			)
			rollout SimpleWaterNormalRollUp "Simple Water Normal" category:17
			(		
				group "Water Normals Map (TexMap)"(
					mapButton mb_waternormal_map "Select  Water Normal Map" width:270 animatable:false
				)
			)	

			parameters AWDShadingParams rollout:AWDShadingRollout
			(
				
				ambientMethod type:#integer default:1 ui:ddl_ambient animateable:false
				diffuseMethod type:#integer default:1 ui:ddl_diffuse animateable:false
				specMethod type:#integer default:1 ui:ddl_spec animateable:false
				normalMethod type:#integer default:1 ui:ddl_normal animateable:false
				
			)
			rollout AWDShadingRollout "AWD Shading Methods" category:18
			(		
				dropDownList ddl_ambient "Ambient" items:#("BasicAmbient", "EnvMapAmbient") animateable:false
				dropDownList ddl_diffuse "Diffuse" items:#("BasicDiffuse", "GradientDiffuse", "WarpDiffuse", "LightMapDiffuse", "CellDiffuse") animateable:false
				dropDownList ddl_spec "Specular" items:#("BasicSpecular", "AnisotropicSpecular", "Phong", "CelSpecular", "FresnelSpecular") animateable:false
				dropDownList ddl_normal "Normal" items:#("BasicAmbient", "SimpleWaterNormal") animateable:false
				
				on ddl_ambient selected  selectedIdx do (	
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)
				on ddl_diffuse selected  selectedIdx do (		
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)
				on ddl_spec selected  selectedIdx do (
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)		
				on ddl_normal selected  selectedIdx do (		
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)
			)	
			parameters AWD_EffectMethodsparams rollout:AWD_EffectMethods 
			(
				
				saveMethods type:#maxObjectTab tabSize:0 tabSizeVariable:true animateable:false
				saveMethodIds type:#stringTab tabSize:0 tabSizeVariable:true animateable:false
			)
			rollout AWD_EffectMethods "AWD Effect Methods Modifiers" category:20
			(
				button btn_new_effectMethod "Create" width:70 pos:[14,10]  animateable:false
				button btn_add_effectMethod "Add" width:70 pos:[90,10] animateable:false
				button btn_select_effectMethod "Select" width:70 pos:[166,10] animateable:false
				button btn_clear_effectMethod "Remove" width:70 pos:[242,10] animateable:false
				listbox method_list "" height:5 items:#()  animateable:false
				on btn_new_effectMethod pressed do (
					thisMesh=selection[1]
					if thisMesh==undefined then(
						messageBox("select one object that should hold the new AWD_EffectMethod and click again!")
						return()
					)
					
					theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDEffectMethod")
					theobj.text ="Add name for new AWDEffectMethod"
					DialogResult = theObj.Showmodal()
					dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
					result = theobj.InstanceName
					thisMod= modPanel.getCurrentObject()
					global AWDEffectMethod
					MyNewModifier = AWDEffectMethod()
					MyNewModifier.name = result
					append saveMethods MyNewModifier
					append saveMethodIds MyNewModifier.thisAWDID
					addmodifier thisMesh MyNewModifier
					method_list.items=#()
					if saveMethods != undefined then (
						for oneMethod in saveMethods do (
							method_list.items= append method_list.items oneMethod.name
						)
					)
					btn_select_effectMethod.enabled=true
					btn_clear_effectMethod.enabled=true
				)
				on btn_add_effectMethod pressed do (
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						chooseEffectMethod mtl
					)
				)
				on btn_select_effectMethod pressed do (
					removeRollout AWD_EffectMethods
					max modify mode 
					modPanel.setCurrentObject saveMethods[method_list.selection]
				)
				
				on btn_clear_effectMethod pressed do (
					if method_list.items.count > 0 and method_list.selection > 0 do(
						saveMethods = deleteItem saveMethods method_list.selection
						saveMethodIds = deleteItem saveMethodIds method_list.selection
						method_list.items = deleteItem method_list.items method_list.selection
					)
					btn_select_effectMethod.enabled=false
					btn_clear_effectMethod.enabled=false
					if method_list.items.count>0 then (
						btn_select_effectMethod.enabled=true
						btn_clear_effectMethod.enabled=true
					)
				)
				
				on AWD_EffectMethods open do (
					method_list.items=#()
					if saveMethods != undefined then (
						for oneMethod in saveMethods do (
							method_list.items= append method_list.items oneMethod.name
						)
					)
					btn_select_effectMethod.enabled=false
					btn_clear_effectMethod.enabled=false
					if method_list.items.count>0 then (
						btn_select_effectMethod.enabled=true
						btn_clear_effectMethod.enabled=true
					)
				)				
			)		
			
			
			parameters AWD_MaterialSettingsparams rollout:AWD_MaterialSettings
			(
				thisAWDID type:#String animateable:false
				hasObjSettings type:#boolean default:true animateable:false
				multiPassMatenabled type:#boolean ui:cbxenable_multiPassMat default:false animateable:false
				multiPassMat type:#boolean ui:cbx_multiPassMat default:false animateable:false
				repeatMatenabled type:#boolean ui:cbxenable_repeatMat default:false animateable:false
				repeatMat type:#boolean ui:cbx_repeatMat default:true animateable:false
				smoothMatenabled type:#boolean ui:cbxenable_smoothMat default:false animateable:false
				smoothMat type:#boolean ui:cbx_smoothMat default:false animateable:false
				mipmapMatenabled type:#boolean ui:cbxenable_mipmapMat default:false animateable:false
				mipmapMat type:#boolean ui:cbx_mipmapMat default:false animateable:false
				alphaBlendingMatenabled type:#boolean ui:cbxenable_alphaBlendingMat default:false animateable:false
				alphaBlendingMat type:#boolean ui:cbx_alphaBlendingMat default:false animateable:false
				alphaTresholdMatenabled type:#boolean ui:cbxenable_alphaTresholdMat default:false animateable:false
				alphaTresholdMat type:#float ui:cbx_alphaTresholdMat default:0.0 animateable:false
				premultipliedMatenabled type:#boolean ui:cbxenable_premultipliedMat  default:false animateable:false
				premultipliedMat type:#boolean ui:cbx_premultipliedMat default:true animateable:false
				blendModeenabled type:#boolean ui:cbxenable_blendMode  default:false animateable:false
				blendMode type:#integer ui:ddl_blendMode default:1 animateable:false
				
				MainUVenabled type:#boolean ui:cbxenable_mainUV  default:false animateable:false
				mainUVChannel type:#integer ui:ddl_mainUV  default:1 animateable:false
				SecUVenabled type:#boolean ui:cbxenable_secUV  default:false animateable:false
				secUVChannel type:#integer ui:ddl_secUV  default:1 animateable:false
				
				AmbientLevelenabled type:#boolean ui:cbxenable_ambientLevel  default:false animateable:false
				ambientLevel type:#float ui:stp_ambientLevel  default:1.0 animateable:false
				SpecularLevelenabled type:#boolean ui:cbxenable_specularLevel  default:false animateable:false
				specularLevel type:#float ui:stp_specularLevel default:1.0 animateable:false
				GlossLevelenabled type:#boolean ui:cbxenable_glossLevel  default:false animateable:false
				glossLevel type:#integer ui:stp_glossLevel default:50		 animateable:false
				
				shadingenabled type:#boolean ui:cbxenable_shading default:false animateable:false
				methodenabled type:#boolean ui:cbxenable_method default:false animateable:false
				
				shadowMod type:#maxObject animateable:false
				shadowModID type:#String default:"" animateable:false
				
			)
			rollout AWD_MaterialSettings "AWD Material Settings" category:19
			(
				label lbl_shading "Shading Methods" pos:[40,10] animateable:false
				checkbox cbxenable_shading "" pos:[10,10] animateable:false
				label lbl_fxMethods "Effect Methods" pos:[210,10] animateable:false
				checkbox cbxenable_method "" pos:[180,10] animateable:false
								
				checkbox cbxenable_multiPassMat "" pos:[10,40] animateable:false
				checkbox cbx_multiPassMat "MultiPass" pos:[40,40] animateable:false
				checkbox cbxenable_repeatMat "" pos:[10,60] animateable:false
				checkbox cbx_repeatMat "Repeat" pos:[40,60] animateable:false
				checkbox cbxenable_smoothMat "" pos:[10,80] animateable:false
				checkbox cbx_smoothMat "Smooth" pos:[40,80] animateable:false
				checkbox cbxenable_mipmapMat "" pos:[10,100] animateable:false
				checkbox cbx_mipmapMat "MipMap" pos:[40,100] animateable:false
				
				checkbox cbxenable_alphaBlendingMat "" pos:[10,120]  animateable:false
				checkbox cbx_alphaBlendingMat "AlpahBlending" pos:[40,120] animateable:false
				checkbox cbxenable_alphaTresholdMat "" pos:[10,140] animateable:false
				label lbl_alphaTresholdMat "AlphaTreshold" pos:[40,140] animateable:false
				Spinner cbx_alphaTresholdMat "" pos:[120,140] width:60 range:[0.0, 1.0, 0.01]  animateable:false
				checkbox cbxenable_premultipliedMat "" pos:[10,160] animateable:false
				checkbox cbx_premultipliedMat "Premultiplied" pos:[40,160] animateable:false
				checkbox cbxenable_blendMode "" pos:[10,180] animateable:false
				label lbl_blendMode "BlendMode" pos:[40,180] animateable:false
				dropdownlist ddl_blendMode ""  pos:[100,178] width:120 items:#("NORMAL", "LAYER", "MULTIPLY", "ADD" , "ALPHA" )  height:6 animateable:false

				checkbox cbxenable_ambientLevel "" pos:[180,40] animateable:false
				label lbl_ambientLevel "Ambient" pos:[210,40]  animateable:false
				Spinner stp_ambientLevel ""  pos:[260, 40] width:60 range:[0.0, 1.0, 0.01] animateable:false
				checkbox cbxenable_specularLevel "" pos:[180,60] animateable:false
				label lbl_specularLevel "Specular" pos:[210,60] animateable:false
				Spinner stp_specularLevel ""  pos:[260, 60] width:60 range:[0.0, 10.0, 0.1] animateable:false
				checkbox cbxenable_glossLevel "" pos:[180,80] animateable:false
				label lbl_glossLevel "Gloss" pos:[210,80] animateable:false
				Spinner stp_glossLevel ""  pos:[260, 80]  range:[0, 1000, 1] type:#integer width:60 animateable:false
				
				checkbox cbxenable_mainUV "" pos:[10,215] animateable:false
				label lbl_mainUV "Main UV" pos:[40,215] animateable:false
				Spinner ddl_mainUV ""  pos:[90,215] width:60 range:[1, 100, 1] type:#integer  animateable:false
				checkbox cbxenable_secUV "" pos:[170,215]   animateable:false
				label lbl_secUV "Second UV" pos:[200,215] animateable:false
				Spinner ddl_secUV ""  pos:[260, 215]  width:60 range:[1, 100, 1] type:#integer animateable:false
				button btn_enableAllProps "Enable all" pos:[20,245] width:60 animateable:false
				button btn_disableAllProps "Disable all" pos:[100,245] width:60 animateable:false
				button btn_setAWDDefaults "Set to AWD-Defaults" pos:[180,245] width:130 animateable:false
				label lbl_UVAnim1 "" pos:[40,260] animateable:false
				group "Shadow (AWDLight-Modiifier)"(
					edittext txt_shadowMod "" readOnly:true animatable:false
					button btn_chooseshadowMod "choose" width:80 across:3 animatable:false
					button btn_selectshadowMod "open"  width:80  animatable:false
					button btn_removeshadowMod "remove"  width:80  animatable:false
				)	
				on cbxenable_method changed thisState do (					
					if methodenabled then 	lbl_fxMethods.enabled=true
					else 	lbl_fxMethods.enabled=false
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)	
				
				on btn_chooseshadowMod pressed do(
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						chooseShadow mtl 
					)					
				)
				on btn_selectshadowMod pressed do(
					max modify mode 
					if shadowMod!=undefined then (
						modPanel.SetCurrentObject shadowMod
					)
				)
				on btn_removeshadowMod pressed do(
					txt_shadowMod.text=""
					shadowMod=undefined
					shadowModID=""
					btn_selectshadowMod.enabled=false
					btn_removeshadowMod.enabled=false
				)		
				
				
				on btn_setAWDDefaults pressed do(
					multiPassMat=false
					repeatMat=true
					smoothMat=false
					mipmapMat=false
					alphaBlendingMat=false
					alphaTresholdMat=0.0
					premultipliedMat=true
					blendMode=1
					mainUVChannel=1
					secUVChannel=1
					ambientLevel=1.0
					specularLevel=1.0
					glossLevel=50
			)
				on btn_enableAllProps pressed do(
					shadingenabled=true
					methodenabled=true
					lbl_fxMethods.enabled=true
					multiPassMatenabled=true
					cbx_multiPassMat.enabled=true
					repeatMatenabled=true
					cbx_repeatMat.enabled=true
					smoothMatenabled=true
					cbx_smoothMat.enabled=true
					mipmapMatenabled=true
					cbx_mipmapMat.enabled=true
					alphaBlendingMatenabled=true
					cbx_alphaBlendingMat.enabled=true
					alphaTresholdMatenabled=true
					cbx_alphaTresholdMat.enabled=true
					lbl_alphaTresholdMat.enabled=true
					premultipliedMatenabled=true
					cbx_premultipliedMat.enabled=true
					blendModeenabled=true
					ddl_blendMode.enabled=true
					lbl_blendMode.enabled=true
					MainUVenabled=true
					ddl_mainUV.enabled=true
					lbl_mainUV.enabled=true
					SecUVenabled=true
					ddl_secUV.enabled=true
					lbl_secUV.enabled=true
					AmbientLevelenabled=true
					stp_ambientLevel.enabled=true
					lbl_ambientLevel.enabled=true
					SpecularLevelenabled=true
					stp_specularLevel.enabled=true
					lbl_specularLevel.enabled=true
					GlossLevelenabled=true
					stp_glossLevel.enabled=true
					lbl_glossLevel.enabled=true
					lbl_shading.enabled=true
					
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)
				
				on btn_disableAllProps pressed do(
					shadingenabled=false
					methodenabled=false
					lbl_fxMethods.enabled=false
					multiPassMatenabled=false
					cbx_multiPassMat.enabled=false
					repeatMatenabled=false
					cbx_repeatMat.enabled=false
					smoothMatenabled=false
					cbx_smoothMat.enabled=false
					mipmapMatenabled=false
					cbx_mipmapMat.enabled=false
					alphaBlendingMatenabled=false
					cbx_alphaBlendingMat.enabled=false
					alphaTresholdMatenabled=false
					cbx_alphaTresholdMat.enabled=false
					lbl_alphaTresholdMat.enabled=false
					premultipliedMatenabled=false
					cbx_premultipliedMat.enabled=false
					blendModeenabled=false
					ddl_blendMode.enabled=false
					lbl_blendMode.enabled=false
					MainUVenabled=false
					ddl_mainUV.enabled=false
					lbl_mainUV.enabled=false
					SecUVenabled=false
					ddl_secUV.enabled=false
					lbl_secUV.enabled=false
					AmbientLevelenabled=false
					stp_ambientLevel.enabled=false
					lbl_ambientLevel.enabled=false
					SpecularLevelenabled=false
					stp_specularLevel.enabled=false
					lbl_specularLevel.enabled=false
					GlossLevelenabled=false
					stp_glossLevel.enabled=false
					lbl_glossLevel.enabled=false
					lbl_shading.enabled=false
					
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)
				on cbxenable_shading changed thisState do (					
					if shadingenabled then (
						lbl_shading.enabled=true
					)
					else (
						lbl_shading.enabled=false
					)
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl
					)
				)
				
				on cbxenable_multiPassMat changed thisState do (
					if multiPassMatenabled then cbx_multiPassMat.enabled=true
					else cbx_multiPassMat.enabled=false
				)
				on cbxenable_repeatMat changed thisState do (
					if repeatMatenabled then cbx_repeatMat.enabled=true
					else cbx_repeatMat.enabled=false
				)
				on cbxenable_smoothMat changed thisState do (
					if smoothMatenabled then cbx_smoothMat.enabled=true
					else cbx_smoothMat.enabled=false
				)
				on cbxenable_mipmapMat changed thisState do (
					if mipmapMatenabled then cbx_mipmapMat.enabled=true
					else cbx_mipmapMat.enabled=false
				)
				on cbxenable_alphaMat changed thisState do (
					if alphaMatenabled then cbx_alphaMat.enabled=true
					else cbx_alphaMat.enabled=false
				)
				on cbxenable_alphaBlendingMat changed thisState do (
					if alphaBlendingMatenabled then cbx_alphaBlendingMat.enabled=true
					else cbx_alphaBlendingMat.enabled=false
				)
				on cbxenable_alphaTresholdMat changed thisState do (
					if alphaTresholdMatenabled then (
						cbx_alphaTresholdMat.enabled=true
						lbl_alphaTresholdMat.enabled=true						
					)
					else (
						cbx_alphaTresholdMat.enabled=false
						lbl_alphaTresholdMat.enabled=false						
					)
				)
				on cbxenable_premultipliedMat changed thisState do (
					if premultipliedMatenabled then cbx_premultipliedMat.enabled=true
					else cbx_premultipliedMat.enabled=false
				)
				on cbxenable_blendMode changed thisState do (
					if blendModeenabled then (
						ddl_blendMode.enabled=true
						lbl_blendMode.enabled=true						
					)
					else(
						ddl_blendMode.enabled=false
						lbl_blendMode.enabled=false						
					)
				)
				on cbxenable_mainUV changed thisState do (
					if MainUVenabled then(
						ddl_mainUV.enabled=true
						lbl_mainUV.enabled=true						
					)
					else (
						ddl_mainUV.enabled=false
						lbl_mainUV.enabled=false						
					)
				)
				on cbxenable_secUV changed thisState do (
					if SecUVenabled then (
						ddl_secUV.enabled=true
						lbl_secUV.enabled=true						
					)
					else (
						ddl_secUV.enabled=false
						lbl_secUV.enabled=false						
					)
				)
				on cbxenable_ambientLevel changed thisState do (
					if AmbientLevelenabled then (
						stp_ambientLevel.enabled=true
						lbl_ambientLevel.enabled=true						
					)
					else (
						stp_ambientLevel.enabled=false
						lbl_ambientLevel.enabled=false						
					)
				)
				on cbxenable_specularLevel changed thisState do (
					if SpecularLevelenabled then (
						stp_specularLevel.enabled=true
						lbl_specularLevel.enabled=true						
					)
					else (
						stp_specularLevel.enabled=false
						lbl_specularLevel.enabled=false						
					)
				)
				on cbxenable_glossLevel changed thisState do (
					if GlossLevelenabled then (
						stp_glossLevel.enabled=true
						lbl_glossLevel.enabled=true						
					)
					else (
						stp_glossLevel.enabled=false
						lbl_glossLevel.enabled=false						
					)
				)
				on AWD_MaterialSettings open do (
					
					btn_selectshadowMod.enabled=false
					btn_removeshadowMod.enabled=false
					
					shadowModID=""
					if shadowMod!=undefined then (						
						shadowModID=shadowMod.thisAWDID
						txt_shadowMod.text=shadowMod.name
						btn_selectshadowMod.enabled=true
						btn_removeshadowMod.enabled=true
					)
					if shadingenabled then lbl_shading.enabled=true
					else lbl_shading.enabled=false				
					if methodenabled then 	lbl_fxMethods.enabled=true
					else 	lbl_fxMethods.enabled=false
					
					mtl=undefined
					if MatEditor.mode ==#basic then 	mtl = meditMaterials[activeMeditSlot] 
					else if MatEditor.mode ==#advanced then 	mtl=sme.GetMtlInParamEditor()		
					if mtl!=undefined then(
						fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\AWDMaterialSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
						showShadingrollups mtl						
					)
					if multiPassMatenabled then cbx_multiPassMat.enabled=true
					else cbx_multiPassMat.enabled=false
					if repeatMatenabled then cbx_repeatMat.enabled=true
					else cbx_repeatMat.enabled=false
					if smoothMatenabled then cbx_smoothMat.enabled=true
					else cbx_smoothMat.enabled=false
					if mipmapMatenabled then cbx_mipmapMat.enabled=true
					else cbx_mipmapMat.enabled=false
					if alphaBlendingMatenabled then cbx_alphaBlendingMat.enabled=true
					else cbx_alphaBlendingMat.enabled=false
					if alphaTresholdMatenabled then (
						cbx_alphaTresholdMat.enabled=true
						lbl_alphaTresholdMat.enabled=true						
					)
					else (
						cbx_alphaTresholdMat.enabled=false
						lbl_alphaTresholdMat.enabled=false						
					)
					if premultipliedMatenabled then cbx_premultipliedMat.enabled=true
					else cbx_premultipliedMat.enabled=false
					if blendModeenabled then (
						ddl_blendMode.enabled=true
						lbl_blendMode.enabled=true						
					)
					else(
						ddl_blendMode.enabled=false
						lbl_blendMode.enabled=false						
					)
					if MainUVenabled then(
						ddl_mainUV.enabled=true
						lbl_mainUV.enabled=true						
					)
					else (
						ddl_mainUV.enabled=false
						lbl_mainUV.enabled=false						
					)
					if SecUVenabled then (
						ddl_secUV.enabled=true
						lbl_secUV.enabled=true						
					)
					else (
						ddl_secUV.enabled=false
						lbl_secUV.enabled=false						
					)
					if AmbientLevelenabled then (
						stp_ambientLevel.enabled=true
						lbl_ambientLevel.enabled=true						
					)
					else (
						stp_ambientLevel.enabled=false
						lbl_ambientLevel.enabled=false						
					)
					if SpecularLevelenabled then (
						stp_specularLevel.enabled=true
						lbl_specularLevel.enabled=true						
					)
					else (
						stp_specularLevel.enabled=false
						lbl_specularLevel.enabled=false						
					)
					if GlossLevelenabled then (
						stp_glossLevel.enabled=true
						lbl_glossLevel.enabled=true						
					)
					else (
						stp_glossLevel.enabled=false
						lbl_glossLevel.enabled=false						
					)
				)
			)		
			
			
			
			
			
			
		)
		custattributes.add thisMat AWDAttributesCA baseobject:true
		global getAWDID
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisMat.thisAWDID=getAWDID()
		print thisMat.thisAWDID
	)	
	return true	
)