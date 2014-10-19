macroScript addAWDTextureSettings
	category:"AWDCommands"
	toolTip:""
(	
	(		
		if not MatEditor.isOpen() then MatEditor.open() 
		thisTexMap=undefined
		--if material mode is "Compact-Mode"
		if MatEditor.mode ==#basic then (
			thisTexMap = meditMaterials[activeMeditSlot] 
		)
		--if material mode is "Slate-Mode"
		else if MatEditor.mode ==#advanced then (
			thisTexMap=sme.GetMtlInParamEditor()		
		)
		--if neither "Compact-Mode" nor "Slate-Mode" (should not happen)
		else (
			messagebox("ERROR:\nCould not read a valid MatEditor Mode\n(Neither 'Compact' cor 'slate')\n")
			return()
		)		
		allValidMaterials=#()
		print (classof thisTexMap as string)
		if classof thisTexMap == BitmapTexture then (
			createdAWD=addAWDTextureSettings thisTexMap
			return true
		)		
		else if classof thisTexMap == Standardmaterial or  classof thisTexMap == Multimaterial then(
			if classof thisTexMap == Standardmaterial then appendIfUnique allValidMaterials thisTexMap
			else if classof thisTexMap == Multimaterial then (
				for mat in thisTexMap.materialIdList do (
					submat = thisTexMap[mat]
					if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
				)
			)
		)
		else (
			answer=queryBox ("No BitmapTexture or Material is selected in the Material-Editor.\nSearch for Material(s) on selected object?")
			if not answer then return ()			
			meshes = selection
			allreadyExisting=0
			wrongTexType=0
			createdTextSettings=0
			if meshes.count>=1 then (
				for i in meshes do(		
					if i.material!=undefined then(
						if classof i.material == Standardmaterial then appendIfUnique allValidMaterials i.material
						else if classof i.material == Multimaterial then (
							for mat in i.material.materialIdList do (
								submat = i.material[mat]
								if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
							)
						)
					)
				)
			)
			else (
				answer=queryBox ("Nothing is selected.\n\n Apply AWDTextureSettings for all textures of all scene-materials ?")
				if not answer then return()	
				--find all CubeMaterials in the meditMaterials
				for mat in meditMaterials do(
					if classof mat == Standardmaterial then appendIfUnique allValidMaterials mat
					else if classof mat == Multimaterial then (
						for matCnt in mat.materialIdList do (
							submat = mat[matCnt]
							if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
						)
					)
				)
				for mat in sceneMaterials do(
					if classof mat == Standardmaterial then appendIfUnique allValidMaterials mat
					else if classof mat == Multimaterial then (
						for matCnt in mat.materialIdList do (
							submat = mat[matCnt]
							if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
						)
					)
				)
				numViews = sme.GetNumViews()
				for numView=1 to numViews do (
					oneView=sme.GetView numView
					numNodes = oneView.GetNumNodes()
					for numNode=1 to numNodes do(
						thisNode= trackViewNodes[#sme][numView][numNode].reference
						if classof mat == Standardmaterial then appendIfUnique allValidMaterials mat
						else if classof mat == Multimaterial then (
							for matCnt in mat.materialIdList do (
								submat = mat[matCnt]
								if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
							)
						)
					)
				)
			)
		)
		createdTextSettings=0
		for mat in allValidMaterials do (			
			if mat.diffuseMap!=undefined then (
				if classof mat.diffuseMap == BitmapTexture then (
					createdAWD=addAWDTextureSettings mat.diffuseMap
					if createdAWD then createdTextSettings+=1												
				)
			)
			if mat.ambientMap!=undefined then (
				if classof mat.ambientMap == BitmapTexture then (
					createdAWD=addAWDTextureSettings mat.ambientMap
					if createdAWD then createdTextSettings+=1												
				)
			)
			if mat.specularMap!=undefined then (
				if classof mat.specularMap == BitmapTexture then (
					createdAWD=addAWDTextureSettings mat.specularMap
					if createdAWD then createdTextSettings+=1												
				)
			)
			if mat.bumpMap!=undefined then (
				if classof mat.bumpMap == BitmapTexture then (
					createdAWD=addAWDTextureSettings mat.specularMap
					if createdAWD then createdTextSettings+=1												
				)
				else if classof mat.bumpMap ==  Normal_Bump then (
					if mat.bumpMap.normal_map!=undefined then(
						if classof mat.bumpMap.normal_map==BitmapTexture then (
							createdAWD=addAWDTextureSettings mat.bumpMap.normal_map
							if createdAWD then createdTextSettings+=1	
						)
					)
				)
			)
		)
		if createdTextSettings==0 then messagebox("Did not add AWDTextureSettings to any BitmapTexture.\n")

	)
)