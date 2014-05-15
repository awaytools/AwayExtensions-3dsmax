
-- add AWDObjectSettings to an object
fn addAWDObjSettings thisObj export = (	
	hasSettings=0
	try hasSettings=thisObj.hasObjSettings
	catch hasSettings=0
	if hasSettings==1 then (
		try(
			thisObj.Export=export
			thisObj.ExportChildren=export
		)
		catch ()
		return false
	)
	else (
		AWDAttributesCA = Attributes AWDAttributesWithAnimation
		(
			parameters AWD_Export rollout:AWD_ObjectSettings
			(
				hasObjSettings type:#integer default:1	--used to check if options exits
				thisAWDID type:#string default:"noIDSet"
				Export type:#boolean ui:CHK_Export default:true
				ExportChildren type:#boolean ui:CHK_Export_Children default:true
				saveExportChildren type:#boolean default:true
			)
			rollout AWD_ObjectSettings "AWD Object Settings" (
				checkbox CHK_Export "Export"
				checkbox CHK_Export_Children "Export Children"
				button btn_addAWDAnimator "Add AWD Animator" width:140
				
				on CHK_Export changed thisState do (	
					if thisState then CHK_Export_Children.enabled=true
					else CHK_Export_Children.enabled=false	
				)
				on btn_addAWDAnimator pressed do (	
					fileIn (getDir #maxRoot + "\\plugins\\Away3dExtensions3dsMax\\AWDObjectSettings.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
					newAnimator selection[1]				
				)
				on AWD_ObjectSettings open do (	
					if thisAWDID=="noIDSet" then thisAWDID=getAWDID()
					thisMesh=selection[1]			
					thisClassStr=classof thisMesh as string
					if thisClassStr=="Editable_mesh" or thisClassStr=="PolyMeshObject" or thisClassStr=="Editable_Poly" then (
						foundAnimator=false
						for m in thisMesh.modifiers do (
							if (classof m as string) == "AWDAnimator" then (
								foundAnimator=true
								exit
							)							
						)
					)
				)					
			)
		)
		custattributes.add thisObj AWDAttributesCA baseobject:true		
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisObj.thisAWDID=getAWDID()
	)	
	thisObj.Export=export
	thisObj.ExportChildren=export
	return true	
)

--create a new AWDAnimatorObject on a object (or throw error if allready exist)
fn newAnimator thisObj = (	
	try(convertToMesh thisObj) 
	catch (
		messageBox ("Object could not be converted to Editable mesh.\n\nNo AWDAnimator created.")
		return undefined
	)
	animatorExists=false
	for m in thisObj.modifiers do (
		if classOf m as string=="AWDAnimator" then(
			animatorExists=true
			exit
		)
	)
	if not animatorExists then(	
		global AWDAnimator
		MyNewModifier = AWDAnimator()
		MyNewModifier.name = "AWD Animator"
		addmodifier thisObj MyNewModifier		
	)
	else	messageBox ("A object can only have one AWDAnimator.modifier assigned.\n\nThere is allready a AWDAnimator-modifer assigned to this object.")
)
