plugin modifier AWDCamera
name:"AWDCamera"
category:"AWDModifier"
classID:#(0x2b393e2f, 0x2bd03e10)
extends:EmptyModifier replaceUI:false version:1
(		
	local main_params
	local camera_params_rollout
	local texture_projector_rollout

	fn getTexProjectUsers = (			

	--global to allow to access vars from dialog-rollout.	
	global availableModsDisplayNames=#()
	global availableModsList=#()	
	global thisMat=(modPanel.GetCurrentObject())
	foundAny=false
	global selectionIdx=1
	selectionIdxCnt=1
	for i in objects do (
		for m in i.modifiers do(
			if (classof m as string)=="AWDEffectMethod" then(
				if m.isProjectiveTextureMethod then(
					if m.pt_projector==thisMat then (
						selectionIdxCnt+=1
						foundAny=true
						appendThis= (appendIfUnique availableModsList m)
						if appendThis then(
							append availableModsDisplayNames ("'"+m.name+"'  object: "+i.name) 
						)
					)
				)
			)
		)
	)
	if availableModsList.count>0 then (
		rollout select_mesh_rollout "AWDTextureProjector Users"	(
			listbox base_name "AWDTextureProjector-Modifier is assigned to this 'Projective-Texture'-AWDEffectMethod-Modifiers:"  height:16 items:availableModsDisplayNames selection:selectionIdx
			on base_name selected selected do (
				modPanel.SetCurrentObject availableModsList[base_name.selection]
				DestroyDialog select_mesh_rollout
			)
		)
		--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
		createDialog select_mesh_rollout 300 240  modal:true
	)
	else	(
		 messagebox("This AWDTextureProjector is not assigned to any  AWDEffectMethod-modifier 'ProjectiveTexture' in the scene")
	)
)
	
	fn showMods thismodifier= (		
		removeRollout thismodifier.camera_params_rollout		
		removeRollout thismodifier.texture_projector_rollout		
		if thismodifier.main_params.cbx_exportCamera.checked then (
			print "camera_params_rollout"
			addRollout thismodifier.camera_params_rollout
		)
		if thismodifier.main_params.cbx_exportTextureProjector.checked then (
			print "texture_projector_rollout"
			addRollout thismodifier.texture_projector_rollout
		)
	)

	parameters camera_params rollout:camera_params_rollout
	(
		lensType type:#integer default:1 ui:ddl_lensType animatable:false 
		projectionHeight type:#integer default:600 ui:spn_projectionHeight animatable:false
		offcenterX_neg type:#integer default:-400 ui:spn_offcenterX_pos animatable:false
		offcenterX_pos type:#integer default:400 ui:spn_offcenterX_neg animatable:false
		offcenterY_pos type:#integer default:-300 ui:spn_offcenterY_pos animatable:false
		offcenterY_neg type:#integer default:300 ui:spn_offcenterY_neg animatable:false
	)
	rollout camera_params_rollout "AWD Camera Properties"  category:2
	(		
		dropdownlist ddl_lensType "Lens Type"  items:#("Perspective", "Orthographic", "Orthographic-OffCenter")  height:4  align:#left animatable:false
		
		spinner spn_projectionHeight "Projection Height "  pos:[10,50] range:[1,4096,1] type:#integer width:120 align:#left animatable:false enabled:false
		spinner spn_offcenterX_pos "X: " pos:[20,50]  range:[-2048,2048,1] type:#integer width:70 align:#left animatable:false  enabled:false
		spinner spn_offcenterX_neg "" pos:[90,50]  range:[-2048,2048,1]  type:#integer width:50 align:#left animatable:false enabled:false
		spinner spn_offcenterY_pos "Y: " pos:[20,75]   range:[-2048,2048,1]  type:#integer width:70 align:#left animatable:false enabled:false
		spinner spn_offcenterY_neg "" pos:[90,75]   range:[-2048,2048,1]  type:#integer width:50 align:#left animatable:false enabled:false
		
		button btn_setDefaults "Set to AWD-Defaults" pos:[10,100] width:140 animateable:false
		on btn_setDefaults pressed do(
					projectionHeight=600
					offcenterX_neg=-400.0
					offcenterX_pos=400					
					offcenterY_pos=300.0
					offcenterY_neg=-300.0
		)		
		
		
		on ddl_lensType selected thisValue do(
			spn_projectionHeight.enabled=false
			spn_offcenterX_pos.enabled=false
			spn_offcenterX_neg.enabled=false
			spn_offcenterY_pos.enabled=false
			spn_offcenterY_neg.enabled=false
			spn_projectionHeight.visible=false
			spn_offcenterX_pos.visible=false
			spn_offcenterX_neg.visible=false
			spn_offcenterY_pos.visible=false
			spn_offcenterY_neg.visible=false
			btn_setDefaults.enabled=false
			btn_setDefaults.visible=false
			if lensType==2 then (
				btn_setDefaults.visible=true
				btn_setDefaults.enabled=true
				spn_projectionHeight.enabled=true
				spn_projectionHeight.visible=true
			)
			if lensType==3 then (
				btn_setDefaults.visible=true
				btn_setDefaults.enabled=true
				spn_offcenterX_pos.enabled=true
				spn_offcenterX_neg.enabled=true
				spn_offcenterY_pos.enabled=true
				spn_offcenterY_neg.enabled=true
				spn_offcenterX_pos.visible=true
				spn_offcenterX_neg.visible=true
				spn_offcenterY_pos.visible=true
				spn_offcenterY_neg.visible=true
			)
		)
		on camera_params_rollout open do(
			spn_projectionHeight.enabled=false
			spn_offcenterX_pos.enabled=false
			spn_offcenterX_neg.enabled=false
			spn_offcenterY_pos.enabled=false
			spn_offcenterY_neg.enabled=false
			spn_projectionHeight.visible=false
			spn_offcenterX_pos.visible=false
			spn_offcenterX_neg.visible=false
			spn_offcenterY_pos.visible=false
			spn_offcenterY_neg.visible=false
			if lensType==2 then (
				spn_projectionHeight.enabled=true
				spn_projectionHeight.visible=true
			)
			if lensType==3 then (
				spn_offcenterX_pos.enabled=true
				spn_offcenterX_neg.enabled=true
				spn_offcenterY_pos.enabled=true
				spn_offcenterY_neg.enabled=true
				spn_offcenterX_pos.visible=true
				spn_offcenterX_neg.visible=true
				spn_offcenterY_pos.visible=true
				spn_offcenterY_neg.visible=true
			)
		)
	)
	parameters texture_projector_params rollout:texture_projector_rollout
	(
		aspect_ratio type:#float default:1.0 ui:spn_aspect_ratio animatable:false
		projectionTexture type:#texturemap ui:set_projectionTexture animatable:false
	)
	rollout texture_projector_rollout "AWD Texture projector" category:3
	(
		spinner spn_aspect_ratio "Aspect ratio "  range:[0.0,10.0,0.01] align:#left animatable:false
		button btn_setDefaultsTexProj "Set to AWD-Defaults" width:140 animateable:false
		button getUsers  "Get users" animateable:false
		on getUsers pressed do(
			getTexProjectUsers()
		)		
		on btn_setDefaultsTexProj pressed do(
					aspect_ratio=1.0
		)		
		group "Texture"(
			mapButton set_projectionTexture "Select Texture" width:130 animatable:false
			button btn_selectTexture "open"  width:60  across:2 animatable:false
			button btn_removeTexture "remove"  width:60  animatable:false
		)
		on  set_projectionTexture picked  this do(
			btn_selectTexture.enabled=false
			btn_removeTexture.enabled=false
			if projectionTexture!=undefined then (
				btn_selectTexture.enabled=true
				btn_removeTexture.enabled=true
			)
		)
		on btn_selectTexture pressed do(
			if not MatEditor.isOpen() then MatEditor.open() 
			if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = projectionTexture
			else if MatEditor.mode ==#advanced then (
				numViews = sme.GetNumViews()
				isFound=false
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference								
						if thisNode == projectionTexture then (
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
					cleanName = "TextureProjector"
					mainName = "TextureProjector"
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
					thisView.CreateNode projectionTexture [0,0]					
				)
			)
		)
		on btn_removeTexture pressed do(
			projectionTexture=undefined
			btn_selectTexture.enabled=false
			btn_removeTexture.enabled=false
		)
		on texture_projector_rollout open do(
			btn_selectTexture.enabled=false
			btn_removeTexture.enabled=false
			if projectionTexture!=undefined then (
				btn_selectTexture.enabled=true
				btn_removeTexture.enabled=true
			)
		)
	)
	
	parameters main rollout:main_params 
	(
		thisAWDID type:#string default:"noIDSet"		 animatable:false
		exportCamera type:#boolean default:true ui:cbx_exportCamera animatable:false
		exportTextureProjector type:#boolean default:false ui:cbx_exportTextureProjector  animatable:false
		
	)
	rollout main_params "AWD Camera Method" category:1
	(	
		checkBox cbx_exportCamera "Export Camera" animatable:false
		checkBox cbx_exportTextureProjector "Export Texture-Projector" animatable:false

		on cbx_exportCamera changed state do(
			curmod=modPanel.GetCurrentObject()
			showMods curmod
		)
		on cbx_exportTextureProjector changed state do(
			curmod=modPanel.GetCurrentObject()
			showMods curmod
		)
		on main_params open do(
			curmod=modPanel.GetCurrentObject()
			showMods curmod
		)
		
	)
	on create do (
		if thisAWDID=="noIDSet" then(
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			thisAWDID=getAWDID()
		)
	)
	on clone orig do (	
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisAWDID=getAWDID()		
	)

)