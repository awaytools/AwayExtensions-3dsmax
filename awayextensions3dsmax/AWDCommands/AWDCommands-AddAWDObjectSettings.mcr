macroScript IncludeSelectedObjects
	category:"AWDCommands"
	toolTip:""
(
	meshes = selection
	if meshes.count>=1 then (
		allreadyExisting=0
		for i in meshes do(		
			createdAWD=addAWDObjSettings i true
		)
	)		
)

macroScript ExcludeSelectedObjects
	category:"AWDCommands"
	toolTip:""
(
	meshes = selection
	if meshes.count>=1 then (
		allreadyExisting=0
		for i in meshes do(		
			createdAWD=addAWDObjSettings i false
		)
	)		
)