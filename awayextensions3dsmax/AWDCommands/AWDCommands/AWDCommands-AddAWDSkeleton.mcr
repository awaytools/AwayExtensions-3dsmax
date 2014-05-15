macroScript AddAWDSkeleton
	category:"AWDCommands"
	toolTip:""

(
	
meshes = selection
if meshes.count==1 then (
	thismesh = meshes[1]
	hasSkeleton=false		
	for m in thismesh.modifiers do(
		if classof  m as string=="AWDSkeleton" or classof  m as string=="AWDSkeletonClone" then(
			hasSkeleton=true
		)
	)
	if not hasSkeleton then (
		theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDSkeleton")
		theobj.text ="Add name for new AWDSkeleton"
		DialogResult = theObj.Showmodal()
		check=dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
		if not check then return()
		result = theobj.InstanceName
		newSkeletonMod=AWDSkeleton()
		newSkeletonMod.name = result
		addModifier thismesh newSkeletonMod
		
	)
	else(
			messagebox("Object allready contains a AWDSkelton Modifier.\n")
			)
			
		)
else if meshes.count>1 then messagebox("AWDSkeleton Modifier can only be attached to one object at a time.\n")		
--if nothing is selected we check if nodes are in the scene
else(
		selected=#()
		for i in objects do(
				for m in i.modifiers do(
					if classof m as string=="AWDSkeleton" or classof m as string=="AWDSkeletonClone" then(
						appendIfUnique selected i
					)				
				)
			)	
		if selected.count>0 then(
			answer=queryBox ("Nothing is selected.\n\n Found "+selected.count  as string+" AWD Skeleton modifiers found in the scene.\n\n Select the Objects that are holding the modifiers ?")
			if answer==true then(
				select selected
				)
			)
		else(
			answer=messageBox ("No Object is selected.\n\n No AWD Skeleton modifiers found in the scene.\n\n To create a AWD skeleton modifier, select the root-bone of your rigg.")
			)
		)
	)