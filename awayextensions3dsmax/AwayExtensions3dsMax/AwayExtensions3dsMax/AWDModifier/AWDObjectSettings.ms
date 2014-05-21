plugin modifier AWDObjectSettings
name:"AWDObjectSettings"
category:"AWDModifier"
classID:#(0x737d7241, 0x568320c1)
extends:EmptyModifier replaceUI:false version:1
(
		
	parameters main rollout:params
	(
		thisAWDID type:#string default:"noIDSet"
		export type:#boolean ui:CBX_export default:true animatable:false
		exportChildren type:#boolean ui:CBX_exportChildren default:true animatable:false
		--nodeTab
	)
	rollout params "AWD Object Settings"
	(
		checkBox CBX_export "Export"
		checkBox CBX_exportChildren "Export Children"
		on CBX_export changed changedState do (
			CBX_exportChildren.enabled=false
			if export then CBX_exportChildren.enabled=true
			)
		on params open do (
			CBX_exportChildren.enabled=false
			if export then CBX_exportChildren.enabled=true
			
		)
	)
	on create do (
		if thisAWDID=="noIDSet" then(
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			thisAWDID=getAWDID()
		)
	)
	on clone orig do (
		--for m in thisObj.modifiers do	if (classof(m) as string)=="AWDAnimator" then exists=true
		--if exists then messagebox("A object can only have on AWDAnimator-Modifier applied.\n\This object allready contains a AWDModifier!")
		fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
		thisAWDID=getAWDID()		
	)
)