plugin modifier AWDShadowMethod
name:"AWDShadowMethod"
category:"AWDModifier"
classID:#(0x201f7e8f, 0x2bd51f77)
extends:EmptyModifier replaceUI:false version:1
(
		
	local params
	local shadows_rollout
	
	fn showMods = (
		if params.cbx_export_shadows.checked then addRollout shadows_rollout
		else removeRollout shadows_rollout
	)

	fn updateUI = (
		thisObj=selection[1]
		if (superclassof thisObj as string)=="light" then(
			if (classof thisObj as string)=="Directionallight" then lightType=2
			else lightType=1
		)
		curMod=modPanel.GetCurrentObject()
		shadows_rollout.ddl_baseShadowMethod.items=#("n.a.")
		shadows_rollout.spn_mapper_cascades.visible=false
		shadows_rollout.spn_mapper_cascades.enabled=false
		shadows_rollout.spn_mapper_coverage.visible=false
		shadows_rollout.spn_mapper_coverage.enabled=false
		shadows_rollout.spn_shadow_samples.enabled=false
		shadows_rollout.spn_shadow_range.enabled=false
		shadows_rollout.ddl_shadowMapper.enabled=false
		shadows_rollout.ddl_shadowMethod.enabled=false
		shadows_rollout.ddl_baseShadowMethod.enabled=false
		
		if lightType==1 then(
			shadows_rollout.ddl_mapper_depthMapSize.items=#("256", "512", "1024")
			shadows_rollout.ddl_shadowMapper.items=#("Cube Map")
			shadows_rollout.ddl_shadowMethod.items=#("Hard - Shadow")
		)
		else if lightType==2 then(
			shadows_rollout.ddl_mapper_depthMapSize.items=#("256", "512", "2048")
			shadows_rollout.ddl_shadowMapper.items=#("Directional", "Cascade", "Near Directional")
			shadows_rollout.ddl_shadowMapper.enabled=true
			if curMod.shadowMapper==1 then (
				shadows_rollout.ddl_shadowMethod.enabled=true
				shadows_rollout.ddl_shadowMethod.items=#("Filtered - Shadow", "Dithered - Shadow", "Soft - Shadow", "Hard - Shadow")
				if curMod.shadowMethod==2 or curMod.shadowMethod==3 then (
					shadows_rollout.spn_shadow_samples.enabled=true
					shadows_rollout.spn_shadow_range.enabled=true
				)
			)
			if curMod.shadowMapper==2 then (
				shadows_rollout.spn_mapper_cascades.visible=true
				shadows_rollout.spn_mapper_cascades.enabled=true
				shadows_rollout.ddl_baseShadowMethod.enabled=true
				shadows_rollout.ddl_shadowMethod.items=#("Cascade - Shadow")
				shadows_rollout.ddl_baseShadowMethod.items=#("Filtered - Shadow", "Dithered - Shadow", "Soft - Shadow", "Hard - Shadow")
				if curMod.baseShadowMethod==2 or curMod.baseShadowMethod==3 then (
					shadows_rollout.spn_shadow_samples.enabled=true
					shadows_rollout.spn_shadow_range.enabled=true
				)
			)
			if curMod.shadowMapper==3 then (
				shadows_rollout.spn_mapper_coverage.visible=true
				shadows_rollout.spn_mapper_coverage.enabled=true
				shadows_rollout.ddl_baseShadowMethod.enabled=true
				shadows_rollout.ddl_shadowMethod.items=#("Near Directional - Shadow")
				shadows_rollout.ddl_baseShadowMethod.items=#("Filtered - Shadow", "Dithered - Shadow", "Soft - Shadow", "Hard - Shadow")
				if curMod.baseShadowMethod==2 or curMod.baseShadowMethod==3 then (
					shadows_rollout.spn_shadow_samples.enabled=true
					shadows_rollout.spn_shadow_range.enabled=true
				)
			)
		)
		if curMod.shadowMapper>shadows_rollout.ddl_shadowMapper.items.Count then curMod.shadowMapper=1
		if curMod.shadowMethod>shadows_rollout.ddl_shadowMethod.items.Count then curMod.shadowMethod=1
		if curMod.baseShadowMethod>shadows_rollout.ddl_baseShadowMethod.items.Count then curMod.baseShadowMethod=1
	)
	
	fn setDefaults = (
		thisObj=selection[1]
		if (superclassof thisObj as string)=="light" then(
			if (classof thisObj as string)=="Directionallight" then lightType=2
			else lightType=1
		)
		curMod=modPanel.GetCurrentObject()
		
		curMod.shadow_alpha=1.0
		curMod.shadow_epsilon=0.02		
		if lightType==1 then(
			curMod.mapper_depthMapSize=2
		)
		else if lightType==2 then(
			curMod.mapper_depthMapSize=3
			if curMod.shadowMapper==1 then (
				if curMod.shadowMethod==2 then (
					curMod.shadow_samples=4
					curMod.shadow_range=1.0
				)
				else if curMod.shadowMethod==3 then (
					curMod.shadow_samples=5
					curMod.shadow_range=2.0
				)
			)
			if curMod.shadowMapper==2 or curMod.shadowMapper==3 then (
				if curMod.baseShadowMethod==2 or curMod.baseShadowMethod==3 then (
					if curMod.baseShadowMethod==2 then (
						curMod.shadow_samples=4
						curMod.shadow_range=1.0
					)
					else if curMod.baseShadowMethod==3 then (
						curMod.shadow_samples=5
						curMod.shadow_range=2.0
					)
				)
			)
			if curMod.shadowMapper==2 then (
				curMod.mapper_cascades=3
			)
			else if curMod.shadowMapper==3 then (
				curMod.mapper_coverage=0.5
			)			
		)
	)
	parameters main rollout:params
	(
		thisAWDID type:#string default:"noIDSet"
		
		use_radius type:#Boolean default:false ui:cbx_use_radius animatable:false	
		this_radius type:#float default:90000 ui:cbx_this_radius animatable:false	
		use_falloff type:#Boolean default:false ui:cbx_use_falloff animatable:false	
		this_falloff type:#float default:100000 ui:cbx_this_falloff animatable:false	
		use_specular type:#Boolean default:false ui:cbx_use_specular animatable:false	
		this_specular type:#float default:1.0 ui:cbx_this_specular animatable:false	
		use_ambient type:#Boolean default:false ui:cbx_use_ambient animatable:false	
		this_ambient type:#float default:0.0 ui:cbx_this_ambient animatable:false		
		this_ambientColor type:#color default:[0,0,0] ui:cpk_ambientColor animatable:false
		use_diffuse type:#Boolean default:false ui:cbx_use_diffuse animatable:false	
		this_diffuse type:#float default:1.0 ui:cbx_this_diffuse animatable:false	

		
		export_shadows type:#Boolean default:true ui:cbx_export_shadows animatable:false	
		
	)
	rollout params "AWD Light"
	(			
		checkbox cbx_use_radius "" pos:[10,10] animateable:false
		spinner cbx_this_radius "Radius   " pos:[40,10] range:[0,999999,1] width:110 animateable:false
		checkbox cbx_use_falloff "" pos:[10,30] animateable:false
		spinner cbx_this_falloff "FallOff   " pos:[40,30] range:[0,999999,1] width:110 animateable:false
		checkBox cbx_use_diffuse "" pos:[10,50]  animatable:false 	
		spinner cbx_this_diffuse "Diffuse   " pos:[40,50] range:[0,100,0.1] width:110 animatable:false 	
		checkbox cbx_use_specular "" pos:[10,70] animateable:false
		spinner cbx_this_specular "Specular" pos:[40,70] range:[0,100,0.1] width:110 animateable:false
		checkBox cbx_use_ambient "" pos:[10,90]  animatable:false 	
		spinner cbx_this_ambient "Ambient" pos:[40,90] range:[0,1.0,0.1] width:110 animatable:false 
		colorpicker cpk_ambientColor "Ambient Color" align:#left animatable:false	
		button btn_enableAllProps "Enable all" pos:[10,140]  width:65 animateable:false
		button btn_disableAllProps "Disable all" pos:[85,140] width:65 animateable:false
		button btn_setDefaults "Set to AWD-Defaults" pos:[10,170] width:140 animateable:false
		checkBox cbx_export_shadows "Export Shadow" pos:[10,200] animatable:false 
		on btn_setDefaults pressed do(
					this_radius=90000
					this_falloff=100000
					this_specular=1.0
					this_ambient=0.0
					this_diffuse=1.0
					this_ambientColor=[255,255,255]
		)		
		on btn_enableAllProps pressed do(
					use_radius=true
					cbx_this_radius.enabled=true
					use_falloff=true
					cbx_this_falloff.enabled=true
					use_specular=true
					cbx_this_specular.enabled=true
					use_ambient=true
					cbx_this_ambient.enabled=true
					use_diffuse=true
					cbx_this_diffuse.enabled=true
					cpk_ambientColor.enabled=true
				)
				
				on btn_disableAllProps pressed do(
					use_radius=false
					cbx_this_radius.enabled=false
					use_falloff=false
					cbx_this_falloff.enabled=false
					use_specular=false
					cbx_this_specular.enabled=false
					use_ambient=false
					cbx_this_ambient.enabled=false
					use_diffuse=false
					cbx_this_diffuse.enabled=false
					cpk_ambientColor.enabled=false
				)
		on cbx_export_shadows changed state do showMods()
		on cbx_use_radius changed thisState do (
			if use_radius then cbx_this_radius.enabled=true
			else cbx_this_radius.enabled=false
		)
		on cbx_use_falloff changed thisState do (
			if use_falloff then cbx_this_falloff.enabled=true
			else cbx_this_falloff.enabled=false
		)
		on cbx_use_specular changed thisState do (
			if use_specular then cbx_this_specular.enabled=true
			else cbx_this_specular.enabled=false
		)
		on cbx_use_ambient changed thisState do (
			cbx_this_ambient.enabled=false
			cpk_ambientColor.enabled=false
			if use_ambient then(
				cbx_this_ambient.enabled=true
				cpk_ambientColor.enabled=true
			)
		)
		on cbx_use_diffuse changed thisState do (
			if use_diffuse then cbx_this_diffuse.enabled=true
			else cbx_this_diffuse.enabled=false
		)
		on params open do (
			showMods()
					if use_radius then cbx_this_radius.enabled=true
					else cbx_this_radius.enabled=false
					if use_falloff then cbx_this_falloff.enabled=true
					else cbx_this_falloff.enabled=false
					if use_specular then cbx_this_specular.enabled=true
					else cbx_this_specular.enabled=false
					cbx_this_ambient.enabled=false
					cpk_ambientColor.enabled=false
					if use_ambient then(
						cbx_this_ambient.enabled=true
						cpk_ambientColor.enabled=true
					)
					if use_diffuse then cbx_this_diffuse.enabled=true
					else cbx_this_diffuse.enabled=false
			)
	)
	parameters shadows_params rollout:shadows_rollout
	(
		
		auto_apply type:#Boolean default:true ui:cbx_auto_apply animatable:false	
		lightType type:#integer default:0		
		shadowMapper type:#integer default:1 ui:ddl_shadowMapper
		mapper_depthMapSize type:#integer default:3 ui:ddl_mapper_depthMapSize
		mapper_cascades type:#integer default:3 ui:spn_mapper_cascades
		mapper_coverage type:#float default:0.5 ui:spn_mapper_coverage
		shadowMethod type:#integer default:1 ui:ddl_shadowMethod
		baseShadowMethod type:#integer default:1 ui:ddl_baseShadowMethod
		shadow_alpha type:#float default:1.0 ui:spn_shadow_alpha
		shadow_epsilon type:#float default:0.02 ui:spn_shadow_epsilon
		shadow_samples type:#integer default:2 ui:spn_shadow_samples
		shadow_range type:#float default:1 ui:spn_shadow_range
		
	)
	rollout shadows_rollout "AWD Shadow Method"
	(		
		dropDownList ddl_shadowMapper "Shadow-Mapper" items:#("Directional", "Near Directional") 
		
		dropDownList ddl_mapper_depthMapSize "Depthmap-size" items:#("Skeleton", "Vertex") 
		spinner spn_mapper_cascades "Cascades "  pos:[20,100] range:[1,4,1] type:#integer align:#left animatable:false enabled:false
		spinner spn_mapper_coverage "Coverage "  pos:[20,100] range:[0.0, 1.0, 0.01] type:#integer align:#left animatable:false enabled:false
		dropDownList ddl_shadowMethod "Shadow-Method" items:#() 
		dropDownList ddl_baseShadowMethod "Base-Method" items:#() 
		
		spinner spn_shadow_alpha "Alpha         "  range:[0.0, 1.0, 0.01] type:#float   align:#left animatable:false 
		spinner spn_shadow_epsilon "Epsilon       " range:[0.0, 1.0, 0.00001] type:#float  align:#left animatable:false 
		spinner spn_shadow_samples "Samples     "   range:[1,24,1] type:#integer align:#left animatable:false enabled:false
		spinner spn_shadow_range "Range        "  range:[1,50,1] type:#float  align:#left animatable:false enabled:false
		button btn_resetDefaults "Reset Defaults"  animatable:false
		checkBox cbx_auto_apply "Auto apply to all lit mats" animatable:false 
		
		on btn_resetDefaults pressed do(
			setDefaults()
		)		
		on ddl_shadowMapper selected thisState do(
			updateUI()
		)
		on ddl_shadowMethod selected thisState do(
			updateUI()
		)
		on ddl_baseShadowMethod selected thisState do(
			updateUI()
		)
		on shadows_rollout open do(
			updateUI()
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