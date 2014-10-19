#include "maxawdexporter.h"

CustomAttributes_struct MaxAWDExporter::GetCustomAWDObjectSettings(IDerivedObject * node_der,Animatable *obj)
{
    CustomAttributes_struct returnData;
    returnData.export_this=true;
    returnData.export_this_children=true;
    if(node_der!=NULL){
        int nMods = node_der->NumModifiers();
        for (int m = 0; m<nMods; m++){
            Modifier* node_mod = node_der->GetModifier(m);
            if (node_mod->IsEnabled()){
                MSTR className;
                node_mod->GetClassName(className);
                char * className_ptr=W2A(className);
                if (ATTREQ(className_ptr,"AWDObjectSettings")){
                    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
                    if(pb!=NULL){
                        int numBlockparams=pb->NumParams();
                        int p=0;
                        for (p=0; p<numBlockparams; p++) {
                            ParamID pid = pb->IndextoID(p);
                            ParamDef def = pb->GetParamDef(pid);
                            ParamType2 paramtype = pb->GetParameterType(pid);
                            char * paramName_ptr=W2A(def.int_name);
                            if (ATTREQ(paramName_ptr, "thisAWDID")){
                                //if (paramtype==TYPE_STRING)
                                //    skeletonMod_ptr=W2A(pb->GetStr(pid));
                            }
                            if (ATTREQ(paramName_ptr, "Export")){
                                if (paramtype==TYPE_BOOL)
                                    returnData.export_this=(0 != pb->GetInt(pid));
                            }
                            if (ATTREQ(paramName_ptr, "ExportChildren")){
                                if (paramtype==TYPE_BOOL)
                                    returnData.export_this_children=(0 != pb->GetInt(pid));
                            }
                        }
                    }
                    free (className_ptr);
                    return returnData;
                }
                free (className_ptr);
            }
        }
        Object * thisOBJ=(Object *)node_der->GetObjRef();
        if(thisOBJ!=NULL){
            if((thisOBJ->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == DERIVOB_CLASS_ID )){
                IDerivedObject* thisDerObj=( IDerivedObject* ) thisOBJ;
                if(thisDerObj!=NULL){
                    int nMods = thisDerObj->NumModifiers();
                    for (int m = 0; m<nMods; m++){
                        Modifier* node_mod = thisDerObj->GetModifier(m);
                        if (node_mod->IsEnabled()){
                            MSTR className;
                            node_mod->GetClassName(className);
                            char * className_ptr=W2A(className);
                            if (ATTREQ(className_ptr,"AWDObjectSettings")){
                                IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
                                if(pb!=NULL){
                                    int numBlockparams=pb->NumParams();
                                    int p=0;
                                    for (p=0; p<numBlockparams; p++) {
                                        ParamID pid = pb->IndextoID(p);
                                        ParamDef def = pb->GetParamDef(pid);
                                        ParamType2 paramtype = pb->GetParameterType(pid);
                                        char * paramName_ptr=W2A(def.int_name);
                                        if (ATTREQ(paramName_ptr, "thisAWDID")){
                                            //if (paramtype==TYPE_STRING)
                                            //    skeletonMod_ptr=W2A(pb->GetStr(pid));
                                        }
                                        if (ATTREQ(paramName_ptr, "export")){
                                            if (paramtype==TYPE_BOOL)
                                                returnData.export_this=(0 != pb->GetInt(pid));
                                        }
                                        if (ATTREQ(paramName_ptr, "exportChildren")){
                                            if (paramtype==TYPE_BOOL)
                                                returnData.export_this_children=(0 != pb->GetInt(pid));
                                        }
                                    }
                                }
                                free (className_ptr);
                                return returnData;
                            }
                            free (className_ptr);
                        }
                    }
                }
            }
        }
    }

    while(obj->SuperClassID() != BASENODE_CLASS_ID) {
        if (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID) {
          IDerivedObject *dobj = (IDerivedObject *)obj;
          obj = dobj->GetObjRef();  // Get next object down mod-stack.
        }
        else {
          break;  // Failed.
        }
    }
    ICustAttribContainer *attributes = obj->GetCustAttribContainer();

    if (attributes) {
        int a=0;
        int numAttribs=0;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            int p=0;
            int t=0;
            CustAttrib *attr = attributes->GetCustAttrib(a);
            for (t=0; t<attr->NumParamBlocks(); t++) {
                IParamBlock2 *block = attr->GetParamBlock(t);
                char * localName_ptr=W2A(block->GetLocalName());
                if (ATTREQ(localName_ptr,"AWD_Export") ){
                    for (p=0; p<block->NumParams(); p++) {
                        ParamID pid = block->IndextoID(p);
                        ParamDef def = block->GetParamDef(pid);
                        char * paramName_ptr=W2A(def.int_name);
                        if (block->GetParameterType(pid)==TYPE_BOOL){
                            if (ATTREQ(paramName_ptr,"Export") )
                                returnData.export_this= (0 != block->GetInt(pid));
                            else if (ATTREQ(paramName_ptr,"ExportChildren") )
                                returnData.export_this_children= (0 != block->GetInt(pid));
                        }
                        free(paramName_ptr);
                    }
                }
                free(localName_ptr);
            }
        }
    }
    return returnData;
}

AWDPrimitive * MaxAWDExporter::ExportPrimitiveGeom(Object * obj, char * name){
    Class_ID classId = obj->ClassID();
    if (classId.PartA() == EDITTRIOBJ_CLASS_ID || classId.PartA() == TRIOBJ_CLASS_ID ){
        return NULL;
    }
    AWD_primitive_type isPrimitve=AWD_PRIMITIVE_UNDEFINED;
    if (classId == Class_ID( BOXOBJ_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_CUBE;
    if (classId == Class_ID( SPHERE_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_SPHERE;
    if (classId == Class_ID( CYLINDER_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_CYLINDER;
    if (classId == PLANE_CLASS_ID)
        isPrimitve=AWD_PRIMITIVE_PLANE;
    if (classId == Class_ID( CONE_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_CONE;
    if (classId == Class_ID( TORUS_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_TORUS;
    if (classId ==PYRAMID_CLASS_ID){}
    if (classId == GSPHERE_CLASS_ID){}
    if (classId == Class_ID( TUBE_CLASS_ID, 0 )){}
    if (classId == Class_ID( HEDRA_CLASS_ID, 0 )){}
    if (classId == Class_ID( BOOLOBJ_CLASS_ID, 0 )){}
    if (isPrimitve!=AWD_PRIMITIVE_UNDEFINED){
        AWDPrimitive *awdGeom = (AWDPrimitive *)primGeocache->Get(obj);
        if (awdGeom == NULL) {
            IParamBlock* pblk = GetParamBlockByIndex((ReferenceMaker* )obj, 0);
            if (pblk){
                if (isPrimitve==AWD_PRIMITIVE_CUBE){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CUBE);
                    awdGeom->set_Yoffset(pblk->GetFloat(BOXOBJ_HEIGHT) /2);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(BOXOBJ_WIDTH) * opts->Scale(), 100);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(BOXOBJ_HEIGHT) * opts->Scale(), 100);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER3, pblk->GetFloat(BOXOBJ_LENGTH) * opts->Scale(), 100);
                    awdGeom->add_bool_property(PROP_PRIM_BOOL1, false, true);
                    int cubeSegX=pblk->GetInt(BOXOBJ_WSEGS);
                    if(cubeSegX>100) cubeSegX=100;
                    int cubeSegY=pblk->GetInt(BOXOBJ_HSEGS);
                    if(cubeSegY>100) cubeSegY=100;
                    int cubeSegZ=pblk->GetInt(BOXOBJ_LSEGS);
                    if(cubeSegZ>100) cubeSegZ=100;
                    awdGeom->add_int_property(PROP_PRIM_INT1, cubeSegX, 1);
                    awdGeom->add_int_property(PROP_PRIM_INT2, cubeSegY, 1);
                    awdGeom->add_int_property(PROP_PRIM_INT3, cubeSegZ, 1);
                }
                if (isPrimitve==AWD_PRIMITIVE_SPHERE){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_SPHERE);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(SPHERE_RADIUS) * opts->Scale(), 50);
                    awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(SPHERE_SEGS), 24);
                    //bool sphere_smooth=pblk->GetInt(SPHERE_SMOOTH);
                    //float sphere_hemisphere=pblk->GetFloat(SPHERE_HEMI);
                    //int sphere_radius=pblk->GetInt(SPHERE_SQUASH);
                    //int sphere_radius=pblk->GetInt(SPHERE_RECENTER);
                }
                if (isPrimitve==AWD_PRIMITIVE_CYLINDER){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CYLINDER);
                    awdGeom->set_Yoffset(pblk->GetFloat(CYLINDER_HEIGHT) /2);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(CYLINDER_RADIUS) * opts->Scale(), 50);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(CYLINDER_RADIUS) * opts->Scale(), 50);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER3, pblk->GetFloat(CYLINDER_HEIGHT) * opts->Scale(), 100);
                    awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(CYLINDER_SIDES), 16);
                    awdGeom->add_int_property(PROP_PRIM_INT2, pblk->GetInt(CYLINDER_SEGMENTS), 1);
                    //int cylinder_capsegments=pblk->GetInt(CYLINDER_CAPSEGMENTS);
                    //bool cylinder_slice_on=pblk->GetInt(CYLINDER_SLICEON);
                    //int box_depth_segs=pblk->GetInt(CYLINDER_SMOOTH);
                }
                if (isPrimitve==AWD_PRIMITIVE_CONE){
                    float cone_radius2=pblk->GetFloat(CONE_RADIUS2);
                    if (cone_radius2==0.0){
                        awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CONE);
                        awdGeom->set_Yoffset(pblk->GetFloat(CONE_HEIGHT) /2);
                        awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(CONE_RADIUS1) * opts->Scale(), 50);
                        awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(CONE_HEIGHT) * opts->Scale(), 100);
                        awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(CONE_SIDES), 16);
                        awdGeom->add_int_property(PROP_PRIM_INT2, pblk->GetFloat(CONE_SEGMENTS), 1);
                    }
                    else{
                        awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CYLINDER);
                        awdGeom->set_Yoffset(pblk->GetFloat(CONE_HEIGHT) /2);
                        awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(CONE_RADIUS2) * opts->Scale(), 50);
                        awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(CONE_RADIUS1) * opts->Scale(), 50);
                        awdGeom->add_number_property(PROP_PRIM_NUMBER3, pblk->GetFloat(CONE_HEIGHT) * opts->Scale(), 100);
                        awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(CONE_SIDES), 16);
                        awdGeom->add_int_property(PROP_PRIM_INT2, pblk->GetInt(CONE_SEGMENTS), 1);
                    }
                    //int cone_capSegments=pblk->GetInt(CONE_CAPSEGMENTS);
                    //int cone_smooth=pblk->GetInt(CONE_SMOOTH);
                    //int cone_slice_on=pblk->GetInt(CONE_SLICEON);
                }
                if (isPrimitve==AWD_PRIMITIVE_CAPSULE){//not in 3dsmax supported...
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CAPSULE);
                }
                if (isPrimitve==AWD_PRIMITIVE_TORUS){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_TORUS);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(TORUS_RADIUS) * opts->Scale(), 10);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(TORUS_RADIUS2) * opts->Scale(), 100);
                    awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(TORUS_SEGMENTS), 16);
                    //float torus_rotation=pblk->GetFloat(TORUS_ROTATION);
                    //float torus_twist=pblk->GetFloat(TORUS_TWIST);
                    //int torus_sides=pblk->GetInt(TORUS_SIDES);
                    //int torus_smooth=pblk->GetInt(TORUS_SMOOTH);
                    //int torus_slice_on=pblk->GetInt(TORUS_SLICEON);
                }
                awd->add_prim_block(awdGeom);
                primGeocache->Set(obj, awdGeom);
                return awdGeom;
            }
            else {
                IParamBlock2* pblk2 = GetParamBlock2ByIndex((ReferenceMaker* )obj, 0);
                if (pblk2){
                    if (isPrimitve==AWD_PRIMITIVE_PLANE){
                        awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_PLANE);
                        int numBlockparams=pblk2->NumParams();
                        int p=0;
                        for (p=0; p<numBlockparams; p++) {
                            ParamID pid = pblk2->IndextoID(p);
                            ParamDef def = pblk2->GetParamDef(pid);
                            ParamType2 paramtype = pblk2->GetParameterType(pid);
                            char * paramName=W2A(def.int_name);
                            if (paramtype==TYPE_FLOAT){
                                if (ATTREQ(paramName, "length"))
                                    awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk2->GetFloat(pid) * opts->Scale(), 100);
                                if (ATTREQ(paramName, "width"))
                                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk2->GetFloat(pid) * opts->Scale(), 100);
                            }
                            if (paramtype==TYPE_INT){
                                if (ATTREQ(paramName, "lengthsegs"))
                                   awdGeom->add_int_property(PROP_PRIM_INT2, pblk2->GetInt(pid), 1);
                                if (ATTREQ(paramName, "widthsegs"))
                                   awdGeom->add_int_property(PROP_PRIM_INT1, pblk2->GetInt(pid), 1);
                            }
                            free(paramName);
                        }
                    }
                    awd->add_prim_block(awdGeom);
                    primGeocache->Set(obj, awdGeom);
                    return awdGeom;
                }
            }
        }
        return awdGeom;
    }
    return NULL;
}

void MaxAWDExporter::ExportUserAttributesForNode(INode *node, Animatable *obj, AWDAttrElement *elem)
{
    if (!opts->ExportAttributes())
        return;
    ExportCustomProps(node, elem);
    BaseObject* node_bo = node->GetObjectRef();
    if((node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == DERIVOB_CLASS_ID ))
    {
        IDerivedObject* node_der = (IDerivedObject*)(node_bo);
        if (node_der!=NULL){
            int nMods = node_der->NumModifiers();
            for (int m = 0; m<nMods; m++){
                Modifier* node_mod = node_der->GetModifier(m);
                if (node_mod->IsEnabled()){
                    MSTR className;
                    node_mod->GetClassName(className);
                    char * className_ptr=W2A(className);
                    if (ATTREQ(className_ptr,"AWDObjectSettings")){
                        ExportUserAttributes(node_mod, elem);
                    }
                    free (className_ptr);
                }
            }
            IDerivedObject* thisBaseObj_der = (IDerivedObject*)(node->GetObjectRef());
            Object * thisOBJ=(Object *)thisBaseObj_der->GetObjRef();
            if((thisOBJ->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == DERIVOB_CLASS_ID )){
                IDerivedObject* thisDerObj=( IDerivedObject* ) thisOBJ;
                if (thisDerObj!=NULL){
                    int nMods = thisDerObj->NumModifiers();
                    for (int m = 0; m<nMods; m++){
                        Modifier* node_mod = thisDerObj->GetModifier(m);
                        if (node_mod->IsEnabled()){
                            MSTR className;
                            node_mod->GetClassName(className);
                            char * className_ptr=W2A(className);
                            if (ATTREQ(className_ptr,"AWDObjectSettings")){
                                ExportUserAttributes(node_mod, elem);
                            }
                            free (className_ptr);
                        }
                    }
                }
            }
        }
    }
    ExportUserAttributes(obj, elem);
}
void MaxAWDExporter::ExportCustomProps(INode *thisNode, AWDAttrElement *elem)
{
    MSTR buffer;
    thisNode->GetUserPropBuffer(buffer);
    AWD_field_type type = AWD_FIELD_STRING;
    awd_uint16 len = 0;
    AWD_field_ptr ptr;
    ptr.v = NULL;
    ptr.str = W2A(buffer);
    len = strlen(ptr.str);
    if (len>0){
        if (ptr.v != NULL) {
            if (ns == NULL) {
                // Namespace has not yet been created; ns is a class
                // variable that will be created only once and then
                // reused for all user attributes.
                char * ns_ptr=opts->AttributeNamespace();//dont free, as this will get freed in the opts delete
                ns = new AWDNamespace(ns_ptr, strlen(ns_ptr));
                awd->add_namespace(ns);
            }
            char * thisName=W2A(_T("CustomProps"));
            elem->set_attr(ns, thisName, strlen(thisName)+1, ptr, len, type);
            free(thisName);
        }
    }
}
void MaxAWDExporter::ExportUserAttributes(Animatable *obj, AWDAttrElement *elem)
{
    if (!opts->ExportAttributes())
        return;

    ICustAttribContainer *attributes = obj->GetCustAttribContainer();
    if (attributes) {
        int a=0;
        int numAttribs=0;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            int k=0;
            CustAttrib *attr = attributes->GetCustAttrib(a);

            for (k=0; k<attr->NumParamBlocks(); k++) {
                int p=0;
                IParamBlock2 *block = attr->GetParamBlock(k);
                if (block!=NULL){
                    char * blockName_ptr=W2A(block->GetLocalName());
                    if (ATTREQ(blockName_ptr,"AWDObjectSettingsParams") ){    }
                    // the next three should not occur yet, as we do not read Custom-properties on materials yet
                    else if (ATTREQ(blockName_ptr,"AWD_MaterialSettingsparams") ){    }
                    else if (ATTREQ(blockName_ptr,"AWD_EffectMethodsparams") ){    }
                    else if (ATTREQ(blockName_ptr,"AWDShadingParams") ){    }
                    else{
                        for (p=0; p<block->NumParams(); p++) {
                            ParamID pid = block->IndextoID(p);
                            Color col;
                            AColor acol;

                            Interval valid = FOREVER;

                            awd_uint16 len = 0;
                            AWD_field_type type = AWD_FIELD_FLOAT32;
                            AWD_field_ptr ptr;
                            ptr.v = NULL;

                            switch (block->GetParameterType(pid)) {
                                case TYPE_ANGLE:
                                case TYPE_PCNT_FRAC:
                                case TYPE_WORLD:
                                case TYPE_FLOAT:
                                    type = AWD_FIELD_FLOAT64;
                                    len = sizeof(awd_float64);
                                    ptr.v = malloc(len);
                                    *ptr.f64 = block->GetFloat(pid);
                                    break;

                                case TYPE_TIMEVALUE:
                                case TYPE_INT:
                                    type = AWD_FIELD_INT32;
                                    len = sizeof(awd_int32);
                                    ptr.v = malloc(len);
                                    *ptr.i32 = block->GetInt(pid);
                                    break;

                                case TYPE_BOOL:
                                    type = AWD_FIELD_BOOL;
                                    len = sizeof(awd_bool);
                                    ptr.v = malloc(len);
                                    *ptr.b = (0 != block->GetInt(pid));
                                    break;

                                case TYPE_FILENAME:
                                case TYPE_STRING:
                                    type = AWD_FIELD_STRING;
                                    ptr.str = (char*)block->GetStr(pid);
                                    len = strlen(ptr.str);
                                    break;

                                case TYPE_RGBA:
                                    type = AWD_FIELD_COLOR;
                                    len = sizeof(awd_color);
                                    col = block->GetColor(pid);
                                    ptr.v = malloc(len);
                                    *ptr.col = awdutil_float_color(col.r, col.g, col.b, 1.0);
                                    break;

                                case TYPE_FRGBA:
                                    type = AWD_FIELD_COLOR;
                                    len = sizeof(awd_color);
                                    acol = block->GetAColor(pid);
                                    ptr.v = malloc(len);
                                    *ptr.col = awdutil_float_color(acol.r, acol.g, acol.b, acol.a);
                                    break;
                            }

                            if (ptr.v != NULL) {
                                ParamDef def = block->GetParamDef(pid);
                                if (ns == NULL) {
                                    // Namespace has not yet been created; ns is a class
                                    // variable that will be created only once and then
                                    // reused for all user attributes.
                                    char * ns_ptr=opts->AttributeNamespace();//dont free, as this will get freed in the opts delete
                                    ns = new AWDNamespace(ns_ptr, strlen(ns_ptr));
                                    awd->add_namespace(ns);
                                }
                                char * thisName=W2A(def.int_name);
                                elem->set_attr(ns, thisName, strlen(thisName)+1, ptr, len, type);
                                free(thisName);
                            }
                        }
                    }
                    free(blockName_ptr);
                }
            }
        }
    }
}

void MaxAWDExporter::CreateDarkLights(){
    if((!opts->IncludeLights())||(!opts->ApplyDarkLight()))
        return;
    AWDMaterial *block;
    AWDBlockIterator *it;
    AWDBlockList * matBlockList = (AWDBlockList *)awd->get_material_blocks();
    if (matBlockList!=NULL){
        if (matBlockList->get_num_blocks()>0){
            it = new AWDBlockIterator(matBlockList);
            while ((block = (AWDMaterial * ) it->next()) != NULL) {
                if(block->get_lightPicker()==NULL){
                    AWDLightPicker * darkLightPicker = awd->CreateDarkLightPicker();
                    if (darkLightPicker!=NULL)
                        block->set_lightPicker(darkLightPicker);
                }
                AWDBlockIterator *itClones;
                AWDMaterial *cloneBlock;
                AWDBlockList * thisBlockList = (AWDBlockList *)block->get_materialClones();
                if((thisBlockList!=NULL)&&(thisBlockList->get_num_blocks()>0)){
                    itClones = new AWDBlockIterator(thisBlockList);
                    while ((cloneBlock = (AWDMaterial * ) it->next()) != NULL) {
                        if(cloneBlock->get_lightPicker()==NULL){
                            AWDLightPicker * darkLightPickerClone = awd->CreateDarkLightPicker();
                            if (darkLightPickerClone!=NULL)
                                cloneBlock->set_lightPicker(darkLightPickerClone);
                        }
                    }
                    delete itClones;
                }
            }
            delete it;
        }
    }
}

void MaxAWDExporter::CopyViewer(bool network)
{
    char awdDrive[4];
    char awdPath[1024];
    char awdName[256];
    char dleFullPath[1024];
    char dleDrive[4];
    char dlePath[1024];
    char tplHtmlPath[1024];
    char tplSwfPath[1024];
    char tplJsPath[1024];
    char outHtmlPath[1024];
    char outSwfPath[1024];
    char outJsPath[1024];

    // Get paths of plug-in DLE file and output AWD file and split into
    // components to be used to concatenate input and output paths.
    //TCHAR * dleFullPath_tchar=A2W(_T(""));
    TCHAR dleFullPath_tchar[1024];
    GetModuleFileName(hInstance, dleFullPath_tchar, 1024);
    char * dleFullPath_ptr=W2A(dleFullPath_tchar);
    _splitpath_s(dleFullPath_ptr, dleDrive, 4, dlePath, 1024, NULL, 0, NULL, 0);
    _splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, awdName, 256, NULL, 0);
    // Select which viewer SWF file to copy depending on which sandbox
    // it should be compiled for (network or local.)
    char *viewerName = network?"AwayExtensions3dsMax\\AWDHTMLViewer\\viewer_n" : "AwayExtensions3dsMax\\AWDHTMLViewer\\viewer_l";

    // Assemble paths for inputs (templates)
    _makepath_s(tplHtmlPath, 1024, dleDrive, dlePath, "AwayExtensions3dsMax\\AWDHTMLViewer\\template", "html");
    _makepath_s(tplSwfPath, 1024, dleDrive, dlePath, viewerName, "swf");
    _makepath_s(tplJsPath, 1024, dleDrive, dlePath, "AwayExtensions3dsMax\\AWDHTMLViewer\\swfobject", "js");

    // Assemble paths for outputs
    _makepath_s(outHtmlPath, 1024, awdDrive, awdPath, awdName, "html");
    _makepath_s(outSwfPath, 1024, awdDrive, awdPath, "viewer", "swf");
    _makepath_s(outJsPath, 1024, awdDrive, awdPath, "swfobject", "js");

    // Copy HTML, and evaluate any variables in the template
    CopyViewerHTML(tplHtmlPath, outHtmlPath, awdName);

    // Copy SWF and JS files as-is
    TCHAR * tplSwfPath_tchar=A2W(tplSwfPath);
    TCHAR * outSwfPath_tchar=A2W(outSwfPath);
    TCHAR * tplJsPath_tchar=A2W(tplJsPath);
    TCHAR * outJsPath_tchar=A2W(outJsPath);
    CopyFile(tplSwfPath_tchar, outSwfPath_tchar, false);
    CopyFile(tplJsPath_tchar, outJsPath_tchar, true);

    TCHAR * outHtmlPath_tchar=A2W(outHtmlPath);
    ShellExecute(NULL, _T("open"), outHtmlPath_tchar, NULL, NULL, SW_SHOWNORMAL);
    free (tplSwfPath_tchar);
    free (outSwfPath_tchar);
    free (tplJsPath_tchar);
    free (outJsPath_tchar);
    free (dleFullPath_ptr);
    free (outHtmlPath_tchar);
}