plugin simpleObject AWDSkyBox
name:"AWDSkyBox"
classID:#(0x626361ea, 0x304c7f72)
category:"AWDObjects"
(
	local thisObj
	
	
fn chooseAWDCubeMaterial thisObj= (				
	global awdCubeMaterialsDisplayNames=#()
	global awdCubeMaterials=#()	
	global thisMod2=thisObj
	global originalIdx=1
	foundAny=false
	cnt=1
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
				thisMod2.params.txt_cubeMat.text = awdCubeMaterials[base_name.selection].name
				thisMod2.material=awdCubeMaterials[base_name.selection]
				thisMod2.params.btn_select.enabled=true
				thisMod2.params.btn_remove.enabled=true
				DestroyDialog select_animSet_rollout
			)
		)
		createDialog select_animSet_rollout 300 240  modal:true
	)
	else	(
		messagebox("No AWDCubeMaterial could be found in the scene")
	)
)	
	
parameters main rollout:params
(
  Size type:#worldUnits ui:size default:0 animatable:false
  facesInverted type:#boolean ui:cbx_facesInverted default:true animatable:false
  SimulateBehaviour type:#boolean ui:cbx_sim_behaviour default:false animatable:false
)
rollout params "AWD SkyBox"
(
    spinner size "Size:" type:#worldunits range:[0,1000,1]
    checkBox cbx_facesInverted "Inverted Faces" 
    checkBox cbx_sim_behaviour "Simulate Away3d SkyBox" 
    label lbl_spacer_1 "" height:5
	group "AWDCubeMaterial"(
		edittext txt_cubeMat "" readOnly:true
		button btn_new "new" width:30 across:4
		button btn_choose "add" width:30 
		button btn_select "sel"  width:30 
		button btn_remove "del"  width:30 
	)
	on btn_new pressed do(
		curMod=modPanel.GetCurrentObject()
		
		theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDCubeMaterial")
		theobj.text ="Add name for new AWDCubeMaterial"
		DialogResult = theObj.Showmodal()
		dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
		result = theobj.InstanceName
		newCubeMat=AWDCubeMaterial()
		newCubeMat.name =	result	
		selection[1].material=newCubeMat
		txt_cubeMat.text=selection[1].material.name
		btn_select.enabled=true
		btn_remove.enabled=true
	)
	on btn_choose pressed do(
		chooseAWDCubeMaterial selection[1]
	)
	on btn_select pressed do(
		if not MatEditor.isOpen() then MatEditor.open() 
		if MatEditor.mode ==#basic then meditMaterials[activeMeditSlot] = selection[1].material
		else if MatEditor.mode ==#advanced then (
			numViews = sme.GetNumViews()
			isFound=false
			for numView=1 to numViews do (
				oneView=sme.GetView numView
				numNodes = oneView.GetNumNodes()
				for numNode=1 to numNodes do(
					thisNode= trackViewNodes[#sme][numView][numNode].reference								
					if thisNode == selection[1].material then (
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
				thisView.CreateNode selection[1].material [0,0]
			)
		)
	)
	on btn_remove pressed do(
		txt_cubeMat.text=""
		selection[1].material=undefined
		btn_select.enabled=false
		btn_remove.enabled=false
	)
	fn simulateAway3dSkyBox = (
		coordSysTM = Inverse(getViewTM())
		viewPt = coordSysTM.row4
		if thisObj!=undefined then (
			thisObj.pos=coordSysTM.row4)
		--print (viewPt  as string)
	)
	fn tcb = simulateAway3dSkyBox()
	on cbx_sim_behaviour changed state do(
		if state then(
			registerRedrawViewsCallback tcb
		)
		else(
			unregisterRedrawViewsCallback tcb
		)
	)
	on params open do (
		btn_select.enabled=false
		btn_remove.enabled=false
		if selection[1]!=undefined then (
			if selection[1].material!=undefined then (
				txt_cubeMat.text=selection[1].material.name
				btn_select.enabled=true
				btn_remove.enabled=true
			)
		)
	)

) --end roll --When Max wants to know whether the object has UV coords, --return the value stored in genMapCoords to inform it of --the current UV sate of the plug-in. on hasUVW do genMapCoords --When 3ds Max tells the plug-in to turn on UV coordinates --(for example when applying a texture --with "Show Map In Viewport" checked), --this handler will automagically check the checkbutton --connected to our parameter called genMapCoords. --Also, when the user changes the checkbox, --the variable will tell the builMesh to generate coordinates. --Note that the handler will override the manual settings, --for example if a texture has "Show Map In Viewport" enabled, --you cannot actually uncheck the "Generate Mapping Coords." --checkbox with the mouse! on setGenUVW bool do genMapCoords = bool 

on buildMesh do ( 
  --Generate a simple Quad 
	negHalfSize=(size/2)
	halfSize=-(size/2)
	if facesInverted then (
		negHalfSize=-(size/2)
		halfSize=(size/2)
	)
	
	local vertex_array = #() 
	append vertex_array [negHalfSize,negHalfSize,halfSize] 
	append vertex_array [halfSize,negHalfSize,halfSize] 
	append vertex_array [halfSize,halfSize,halfSize] 
	append vertex_array [negHalfSize,halfSize,halfSize]  
	
	append vertex_array [negHalfSize,negHalfSize,negHalfSize] 
	append vertex_array [halfSize,negHalfSize,negHalfSize] 
	append vertex_array [halfSize,halfSize,negHalfSize] 
	append vertex_array [negHalfSize,halfSize,negHalfSize]  
	
	append vertex_array [halfSize,negHalfSize,negHalfSize] 
	append vertex_array [halfSize,negHalfSize,halfSize] 
	append vertex_array [halfSize,halfSize,halfSize] 
	append vertex_array [halfSize,halfSize,negHalfSize]  
	
	append vertex_array [negHalfSize,negHalfSize,negHalfSize] 
	append vertex_array [negHalfSize,negHalfSize,halfSize] 
	append vertex_array [negHalfSize,halfSize,halfSize] 
	append vertex_array [negHalfSize,halfSize,negHalfSize]  
	
	append vertex_array [negHalfSize,halfSize,negHalfSize] 
	append vertex_array [negHalfSize,halfSize,halfSize] 
	append vertex_array [halfSize,halfSize,halfSize] 
	append vertex_array [halfSize,halfSize,negHalfSize]  
	
	append vertex_array [negHalfSize,negHalfSize,negHalfSize] 
	append vertex_array [negHalfSize,negHalfSize,halfSize] 
	append vertex_array [halfSize,negHalfSize,halfSize] 
	append vertex_array [halfSize,negHalfSize,negHalfSize]  
	face_array = #( [3,2,1], [4,3,1] ,   [5,6,7], [5,7,8],   [9,10,11], [9,11,12],   [15,14,13], [16,15,13],   [19,18,17], [20,19,17],   [21,22,23], [21,23,24]) 
	setMesh mesh verts:vertex_array faces:face_array
	
	setfacematid mesh 1 1 --top
	setfacematid mesh 2 1
	setfacematid mesh 3 2 --down
	setfacematid mesh 4 2
	setfacematid mesh 5 3 --posX
	setfacematid mesh 6 3
	setfacematid mesh 7 4 --neg X
	setfacematid mesh 8 4
	setfacematid mesh 9 5 --pos Z
	setfacematid mesh 10 5
	setfacematid mesh 11 6 -- negZ
	setfacematid mesh 12 6
	u_tile=1.0
	v_tile=1.0
    local uvw_array = #() 
	append uvw_array [u_tile ,v_tile ,0.0] 
	append uvw_array [0.0,v_tile ,0.0]
	append uvw_array [0.0,0.0,0.0] 
	append uvw_array [u_tile ,0.0,0.0] 
	
	append uvw_array [0.0,0.0,0.0] 
	append uvw_array [u_tile ,0.0,0.0] 
	append uvw_array [u_tile ,v_tile ,0.0] 
	append uvw_array [0.0,v_tile ,0.0]
	
	append uvw_array [0.0,0.0,0.0] 
	append uvw_array [0.0,v_tile ,0.0]
	append uvw_array [u_tile ,v_tile ,0.0] 
	append uvw_array [u_tile ,0.0,0.0] 
	
	append uvw_array [u_tile ,0.0,0.0] 
	append uvw_array [u_tile ,v_tile ,0.0] 
	append uvw_array [0.0,v_tile ,0.0]
	append uvw_array [0.0,0.0,0.0] 
	
	append uvw_array [u_tile ,0.0,0.0] 
	append uvw_array [u_tile ,v_tile ,0.0] 
	append uvw_array [0.0,v_tile ,0.0]
	append uvw_array [0.0,0.0,0.0] 
	
	append uvw_array [0.0,0.0,0.0] 
	append uvw_array [0.0,v_tile ,0.0]
	append uvw_array [u_tile ,v_tile ,0.0] 
	append uvw_array [u_tile ,0.0,0.0] 
	
	--set the number of vertices 
	setNumTVerts mesh uvw_array.count 
	--set all vertices 
	for v = 1 to uvw_array.count do setTVert mesh v uvw_array[v] 
	--build texture faces 
	buildTVFaces mesh false 
	--set all texture faces (in this case using the mesh faces
	--since we happen to have a one-to-one correspondence
	--between vertices and texture vertices.) 
	--This is not always the case though... 
	for f = 1 to face_array.count do setTVFace mesh f face_array[f]  
	thisObj=selection[1]
	--mesh.backFaceCull=true
 --set the number of vertices 
	)--end on buildMesh 
tool create 
( 
  on mousePoint click do 
  case click of 
  ( 
    1: ( nodeTM.translation = gridPoint ) 
    2: ( #stop) 
  ) 
  on mouseMove click do 
  case click of 
  ( 
    2: (Size= abs gridDist.x) 
  ) 
  ) 
)--end plugin