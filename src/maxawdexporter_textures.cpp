#include "maxawdexporter.h"

CustomTexSettings_struct MaxAWDExporter::GetCustomAWDTextureSettings(BitmapTex *tex)
{
    CustomTexSettings_struct returnData;
    returnData.replace=false;
    returnData.replaceURL="";
    returnData.useUvAnim=false;
    returnData.thisID="";
    returnData.thisID_len=0;
    returnData.texType=UNDEFINEDTEXTYPE;
    ICustAttribContainer *attributes = tex->GetCustAttribContainer();
    char * tex_awd_id = NULL;
    bool useUVAnim=false;
    bool autoanim=false;
    StdUVGen * uvGen=tex->GetUVGen();
    if (attributes) {
        int a=0;
        int numAttribs;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            CustAttrib *attr = attributes->GetCustAttrib(a);
            int t=0;
            for (t=0; t<attr->NumParamBlocks(); t++) {
                IParamBlock2 *block = attr->GetParamBlock(t);
                char * localName_ptr=W2A(block->GetLocalName());
                if (ATTREQ(localName_ptr,"AWD_TextureSettingsparams") ){
                    int p=0;
                    for (p=0; p<block->NumParams(); p++) {
                        ParamID pid = block->IndextoID(p);
                        ParamDef def = block->GetParamDef(pid);
                        char * paramName_ptr=W2A(def.int_name);
                        if (block->GetParameterType(pid)==TYPE_BOOL){
                            if (ATTREQ(paramName_ptr,"saveReplace") )
                                returnData.replace= (0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"uvanimeabled"))
                                useUVAnim=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"simpleMode"))
                                autoanim=(0 != block->GetInt(pid));
                        }
                        if ((block->GetParameterType(pid)==TYPE_STRING)||(block->GetParameterType(pid)==TYPE_FILENAME)){
                            if (ATTREQ(paramName_ptr,"saveReplaceURL") )
                                returnData.replaceURL = W2A(block->GetStr(pid));
                            if (ATTREQ(paramName_ptr, "thisAWDID")){
                                returnData.thisID = W2A(block->GetStr(pid));
                                returnData.thisID_len = strlen(returnData.thisID);
                            }
                        }
                        if (block->GetParameterType(pid)==TYPE_INT){
                            if (ATTREQ(paramName_ptr,"saveTexType") ){
                                int typeInt=block->GetInt(pid);
                                if (typeInt==2){
                                    returnData.relative = true;
                                    returnData.texType = EXTERNAL;
                                }
                                if (typeInt==3){
                                    returnData.relative = false;
                                    returnData.texType = EXTERNAL;
                                }
                                if (typeInt==4)
                                    returnData.texType = EMBEDDED;
                            }
                        }
                        free(paramName_ptr);
                    }
                }
            }
            if((returnData.thisID !=NULL)&&(useUVAnim)&&(uvGen!=NULL)){
                for (t=0; t<attr->NumParamBlocks(); t++) {
                    IParamBlock2 *block = attr->GetParamBlock(t);
                    char * localName_ptr=W2A(block->GetLocalName());
                    if (ATTREQ(localName_ptr,"AnimClipsparams") ){
                        sourceObjsIDsCache->Set(returnData.thisID, uvGen);
                        AWDBlockList * thisSourceClips=(AWDBlockList *)animSourceForAnimSet->Get(returnData.thisID );
                        //output_debug_string(vertexAnimMod_ptr);
                        if (thisSourceClips==NULL){
                            thisSourceClips = new AWDBlockList();
                            animSourceForAnimSet->Set(returnData.thisID , thisSourceClips);
                        }
                        ReadAWDAnimationClips(block, returnData.thisID , thisSourceClips, ANIMTYPEUV);
                        if(thisSourceClips->get_num_blocks()>0){
                            returnData.useUvAnim=useUVAnim;
                        }
                    }
                }
            }
        }
    }
    if(!autoanim)
        returnData.useUvAnim=false;
    if(!returnData.useUvAnim){
        if(returnData.thisID_len >0)
           free(returnData.thisID);
        returnData.thisID=NULL;
        returnData.thisID_len=0;
    }
    return returnData;
}

AWDBitmapTexture * MaxAWDExporter::ExportBitmapTexture(BitmapTex *tex, AWDMaterial * curMat, AWD_tex_type cubeTexType, AWDTexPurpose_type tex_purpose)
{
    //output_debug_string(" -->ExportBitmapTexture");
    AWDBitmapTexture *awdTex;
    if(tex!=NULL){
        char * texname_ptr=W2A(tex->GetName());
        CustomTexSettings_struct customTexSettings=GetCustomAWDTextureSettings(tex);
        bool replaceBitmap=false;
        MaxSDK::AssetManagement::AssetUser asset = tex->GetMap();
        char * absTexPath_ptr = W2A(asset.GetFullFilePath());
        if (customTexSettings.replace){
            if (strlen(customTexSettings.replaceURL)==0){
                AWDMessageBlock * newWarning = new AWDMessageBlock(texname_ptr, "- There are AWDTextureSettings that say the texture should be replaced on export, but no url is set.");
                awd->get_message_blocks()->append(newWarning);}
            else{
                free(absTexPath_ptr);
                //output_debug_string(customTexSettings.replaceURL);
                absTexPath_ptr = customTexSettings.replaceURL;
                //output_debug_string(absTexPath_ptr);
                replaceBitmap=true;
            }
        }
        /* TODO:
IMPLEMENT UV Tile ?
int tillingFlag=uvGen->GetTextureTiling();
*/
        StdUVGen * uvGen=tex->GetUVGen();
        if ((curMat!=NULL)&&(uvGen!=NULL)){
            if((uvGen->GetUAng(0)!=0.0f)||(uvGen->GetVAng(0)!=0.0f)){
                AWDMessageBlock * newWarning = new AWDMessageBlock(texname_ptr, "- UV Rotation might not show up correctly in Away3d.");
                awd->get_message_blocks()->append(newWarning);
            }
            if (curMat->get_uv_transform_mtx()==NULL){
                double *uvmtxData = (double *)malloc(6*sizeof(double));
                uvmtxData[0]=uvGen->GetUScl(0);//a:scalex
                uvmtxData[1]=(uvGen->GetAng(0));//b:rot1
                uvmtxData[2]=(uvGen->GetAng(0)*-1);//d:rot2
                uvmtxData[3]=uvGen->GetVScl(0);//c:scaley
                uvmtxData[4]=uvGen->GetUOffs(0); //0.0;//tx:offsetx
                uvmtxData[5]=uvGen->GetVOffs(0); //0.0;//ty:offsety
                curMat->set_uv_transform_mtx(uvmtxData);
                free(uvmtxData);
            }
            int mapChannel=uvGen->GetMapChannel();
            if (curMat->get_mappingChannel()==0){
                curMat->set_mappingChannel(mapChannel);
            }
            else if (curMat->get_mappingChannel()!=mapChannel){
                AWDMessageBlock * newWarning = new AWDMessageBlock(texname_ptr, "- All Textures of a material should use the same mapping channel.");
                awd->get_message_blocks()->append(newWarning);
            }
        }

        AWD_tex_type texType=UNDEFINEDTEXTYPE;
        bool copyTxt=true;
        bool forceBaseName=true;

        // if no saveType is requested (for cubeTex), we check for the saveType
        if (cubeTexType==UNDEFINEDTEXTYPE){
            if (customTexSettings.texType!=UNDEFINEDTEXTYPE){
                texType=customTexSettings.texType;
                copyTxt=true;
                forceBaseName=customTexSettings.relative;
            }
            else{
                int saveType=opts->TextureMode();
                if (saveType==1)
                    texType=EMBEDDED;
                if (saveType==2){
                    copyTxt=false;
                    forceBaseName=false;
                    texType=EXTERNAL;
                }
                if (saveType==3){
                    copyTxt=true;
                    forceBaseName=true;
                    texType=EXTERNAL;
                }
            }
        }
        else{
            texType=cubeTexType;
        }
        // now we have the abosute pathname of the file to export.
        // we check if a block exists, and if a saveMode was requested, we check if it fits
        if (strlen(absTexPath_ptr)==0){
            AWDMessageBlock * newWarning = new AWDMessageBlock(texname_ptr, "Path of texture is empty. Could not find the texture");
            awd->get_message_blocks()->append(newWarning);
            return NULL;
        }
        awdTex = (AWDBitmapTexture *)textureCache->Get(absTexPath_ptr);
        bool isWrongMode=false;
        if (cubeTexType!=UNDEFINEDTEXTYPE){
            if (awdTex != NULL){
                if(awdTex->get_tex_type()!=texType){
                    isWrongMode=true;
                }
            }
        }
        if ((awdTex == NULL) || (isWrongMode)) {
            if((isWrongMode)&&(awdTex != NULL)&&((tex_purpose==FORCUBETEXTURE)||(tex_purpose==FXLIGHTMAP))&&((awdTex->get_height()>1024)||(awdTex->get_width()>1024))){
                AWDMessageBlock * newWarning = new AWDMessageBlock(texname_ptr, "Texture is to big for its purpose (CubeMap and Lightmap only support textures up to 1024 x 1024)");
                awd->get_message_blocks()->append(newWarning);
                return NULL;
            }

            if (!isWrongMode){
                awdTex = new AWDBitmapTexture(texname_ptr, strlen(texname_ptr));
                int saveType=opts->TextureMode();
                if (saveType==0){
                    awdTex->set_tex_type(EMBEDDED);
                    awdTex->make_invalide();
                    return awdTex;
                }
            }
            if (customTexSettings.useUvAnim){
                if(customTexSettings.thisID_len>0){
                    awdTex->set_uvAnimSourceId(customTexSettings.thisID, strlen(customTexSettings.thisID));
                }
                else{
                    AWDMessageBlock * newWarning = new AWDMessageBlock(texname_ptr, "- ERROR: AWDTextureSettings saying that UVAnimation should be used for this texmap, but no AWDID could be read.");
                    awd->get_message_blocks()->append(newWarning);
                }
            }
            bool bitMapValid=true;
            bool cubeBitMapValid=true;
            bool validSize=true;

            // only check if the bitmap is valid, if the path was not replaced earlier
            if (!replaceBitmap){
                Bitmap *thisbitmap;
                try{
                    thisbitmap=tex->GetBitmap(0);
                    int width=thisbitmap->Width();
                    int height=thisbitmap->Height();
                    if ((width!=2)&&(width!=4)&&(width!=8)&&(width!=16)&&(width!=32)&&(width!=64)&&(width!=128)&&(width!=256)&&(width!=512)&&(width!=1024)&&(width!=2048)&&(width!=4096)){
                        if ((tex_purpose==FORCUBETEXTURE)){
                            cubeBitMapValid=false;
                            bitMapValid=false;
                            validSize=false;
                        }
                        else if(tex_purpose==FXLIGHTMAP){
                            if((width!=2048)){
                                bitMapValid=false;
                                validSize=false;
                            }
                        }
                        else{
                            if((width!=2048)&&(width!=4096)){
                                bitMapValid=false;
                                validSize=false;
                            }
                        }
                    }
                    if ((height!=2)&&(height!=4)&&(height!=8)&&(height!=16)&&(height!=32)&&(height!=64)&&(height!=128)&&(height!=256)&&(height!=512)&&(height!=1024)){
                        if ((tex_purpose==FORCUBETEXTURE)){
                            cubeBitMapValid=false;
                            bitMapValid=false;
                            validSize=false;
                        }
                        else if(tex_purpose==FXLIGHTMAP){
                            if((height!=2048)){
                                bitMapValid=false;
                                validSize=false;
                            }
                        }
                        else{
                            if((height!=2048)&&(height!=4096)){
                                bitMapValid=false;
                                validSize=false;
                            }
                        }
                    }
                }
                catch(...){bitMapValid=false;}
                if(!bitMapValid){
                    AWDMessageBlock * newWarning;
                    if (validSize)
                        newWarning = new AWDMessageBlock(texname_ptr, "Error reading size of texture");
                    else{
                        if(cubeBitMapValid)
                            newWarning = new AWDMessageBlock(texname_ptr, "Texture is not Power of 2 (incorrect size)");
                        else
                            newWarning = new AWDMessageBlock(texname_ptr, "Texture is not Power of 2 (incorrect size) or to big for its purpose (CubeMap and Lightmap only support textures up to 1024 x 1024)");
                    }
                    awd->get_message_blocks()->append(newWarning);
                }
            }
            //TODO: get the type of the bitmap (is it jpg or png ?)
            // if the bitmap is not JPG or PNG, either say its invalid, or find a way to convert the bitmap by the 3dsmax sdk....

            //char * thisExtension=(char *)malloc(4);
            char thisExtension[16];
            // Split path to retrieve name and concatenate to form base name
            _splitpath_s(absTexPath_ptr, NULL, 0, NULL, 0, NULL, 0, thisExtension, 16);

            if ((ATTREQ(thisExtension, ".jpg"))||(ATTREQ(thisExtension, ".JPG"))
                ||(ATTREQ(thisExtension, ".jpeg"))||(ATTREQ(thisExtension, ".JPEG"))
                ||(ATTREQ(thisExtension, ".png"))||(ATTREQ(thisExtension, ".PNG"))
                ||(ATTREQ(thisExtension, ".atf"))||(ATTREQ(thisExtension, ".ATF"))) {
            }
            else{
                AWDMessageBlock * newWarning = new AWDMessageBlock(texname_ptr, "unsupported texture-type. file is not jpg, png or atf");
                awd->get_message_blocks()->append(newWarning);
                bitMapValid=false;
            }
            free(texname_ptr);
            if (!bitMapValid)
                awdTex->make_invalide();
            else {
                if (texType==EMBEDDED) {
                    int fd = open(absTexPath_ptr, _O_BINARY | _O_RDONLY);

                    if (fd >= 0) {
                        struct stat fst;
                        fstat(fd, &fst);

                        awd_uint8 *buf = (awd_uint8*)malloc(fst.st_size);
                        read(fd, buf, fst.st_size);
                        close(fd);

                        awdTex->set_tex_type(EMBEDDED);
                        awdTex->set_embed_data(buf, fst.st_size);
                    }
                    else {
                        char buf[1024];
                        snprintf(buf, 1024,
                            "Texture \"%s\" could not be opened for embedding. "
                            "The file might be missing. Correct the path and try exporting again.",
                            tex->GetName());

                        // Show error message and return to stop texture export.
                        DieWithErrorMessage(buf, "Texture embedding error");
                        return NULL;
                    }
                }
                else {
                    awdTex->set_tex_type(EXTERNAL);

                    if (forceBaseName) {
                        char fileName[256];
                        char fileExt[16];
                        char *url;

                        // Split path to retrieve name and concatenate to form base name
                        _splitpath_s(absTexPath_ptr, NULL, 0, NULL, 0, fileName, 240, fileExt, 16);

                        if (!copyTxt) {
                            url = (char*)malloc(strlen(fileName)+strlen(fileExt)+1);
                            strcpy(url, fileName);
                            strcat(url, fileExt);
                            awdTex->set_url(url, strlen(fileName)+strlen(fileExt));
                        }

                        else{
                            url = (char*)malloc(strlen("textures/")+strlen(fileName)+strlen(fileExt)+1);
                            strcpy(url, "textures/");
                            strcat(url, fileName);
                            strcat(url, fileExt);
                            awdTex->set_url(url, strlen("textures/")+strlen(fileName)+strlen(fileExt));
                            char awdDrive[4];
                            char awdPath[1024];
                            char outPath[1024];
                            char outPathDir[1024];
                            try{
                                // Concatenate output path using base path of AWD file and basename of
                                // texture file, and copy texture file to output directory.
                                _splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, NULL, 0, NULL, 0);

                                //add the "textures" folder to the output directory
                                strcat(awdPath, "textures");
                                _makepath_s(outPathDir, 1024, awdDrive, awdPath, NULL, NULL);
                                mkdir(outPathDir);

                                //copy the bitmap to the "textures" folder
                                _makepath_s(outPath, 1024, awdDrive, awdPath, fileName, fileExt);
                                CopyFileA(absTexPath_ptr, outPath, true);
                            }
                            catch(...){
                                awdTex->set_tex_type(UNDEFINEDTEXTYPE);
                                //error: something went wrong when copying the file
                            }
                        }
                    }
                    else {
                        awdTex->set_url(absTexPath_ptr, strlen(absTexPath_ptr));
                    }
                }
            }
            //create and add the texture-block no matter if the bitmap is valid or not.
            //this way we prevent the exporter to try to validate the same (invalid) bitmap multiple times
            textureCache->Set(absTexPath_ptr,awdTex);
            //if (cubeTexType==UNDEFINEDTEXTYPE)
            awd->add_texture(awdTex);
        }
        free(absTexPath_ptr);
        return awdTex;
    }
    return NULL;
}

AWDCubeTexture * MaxAWDExporter::ExportAWDCubeTexure(MultiMtl * awdCubeMat)
{
    AWDCubeTexture * cubeTex = (AWDCubeTexture * )cubeMatCache->Get(awdCubeMat);
    if (cubeTex==NULL){
        char * cubetexName_ptr=W2A(awdCubeMat->GetName());
        cubeTex = new AWDCubeTexture(cubetexName_ptr, strlen(cubetexName_ptr));
        awd->add_cube_texture(cubeTex);
        cubeMatCache->Set(awdCubeMat , cubeTex);

        int num_params = awdCubeMat->NumParamBlocks();
        int p=0;
        int x=0;
        BitmapTex * upTex = NULL;
        BitmapTex * downTex = NULL;
        BitmapTex * leftTex = NULL;
        BitmapTex * rightTex = NULL;
        BitmapTex * frontTex = NULL;
        BitmapTex * backTex = NULL;
        bool replacetex = false;
        char * replaceURL_ptr=NULL;
        int saveType=0;
        for (x=0; x<awdCubeMat->NumParamBlocks(); x++) {
            IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)awdCubeMat, x);
            for (p=0; p<pb->NumParams(); p++) {
                ParamID pid = pb->IndextoID(p);
                ParamDef def = pb->GetParamDef(pid);
                char * paramName_ptr=W2A(def.int_name);
                if (pb->GetParameterType(pid)==TYPE_BOOL){
                    if (ATTREQ(paramName_ptr,"saveReplace"))
                        replacetex = (0 != pb->GetInt(pid));
                }
                if (pb->GetParameterType(pid)==TYPE_STRING){
                    if (ATTREQ(paramName_ptr,"saveReplaceURL"))
                        replaceURL_ptr = W2A(pb->GetStr(pid));
                }
                if (pb->GetParameterType(pid)==TYPE_INT){
                    if (ATTREQ(paramName_ptr,"saveTexType")){
                        int texType=pb->GetInt(pid);
                        if (texType==2)
                            cubeTex->set_tex_type(EXTERNAL);
                        if (texType==3)
                            cubeTex->set_tex_type(EMBEDDED);
                    }
                }
                if (pb->GetParameterType(pid)==TYPE_TEXMAP){
                    if (ATTREQ(paramName_ptr,"upSave"))
                        upTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"downSave"))
                        downTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"leftSave"))
                        leftTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"rightSave"))
                        rightTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"frontSave"))
                        frontTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"backSave"))
                        backTex = (BitmapTex*)pb->GetTexmap(pid);
                }
                free(paramName_ptr);
            }
        }
        if (cubeTex->get_tex_type()==UNDEFINEDTEXTYPE){
            int saveType=opts->TextureMode();
            if (saveType==1)
                cubeTex->set_tex_type(EMBEDDED);
            if (saveType==2)
                cubeTex->set_tex_type(EXTERNAL);
            if (saveType==3)
                cubeTex->set_tex_type(EXTERNAL);
        }

        if (!replacetex){
            if((upTex==NULL)||(downTex==NULL)||(leftTex==NULL)||(rightTex==NULL)||(frontTex==NULL)||(backTex==NULL)){
                AWDMessageBlock * newWarning = new AWDMessageBlock(cubetexName_ptr, "AWDCubeTexture has not 6 valid materials applied. CubeTexture will not be exported");
                awd->get_message_blocks()->append(newWarning);
                cubeTex->make_invalide();
            }
            else{
                AWDBitmapTexture * leftTex_ptr=ExportBitmapTexture(leftTex, NULL, cubeTex->get_tex_type(), FORCUBETEXTURE);
                if(leftTex_ptr!=NULL)
                    cubeTex->get_texture_blocks()->force_append(leftTex_ptr);
                AWDBitmapTexture * rightTex_ptr=ExportBitmapTexture(rightTex, NULL, cubeTex->get_tex_type(), FORCUBETEXTURE);
                if(rightTex_ptr!=NULL)
                    cubeTex->get_texture_blocks()->force_append(rightTex_ptr);
                AWDBitmapTexture * upTex_ptr=ExportBitmapTexture(upTex, NULL, cubeTex->get_tex_type(), FORCUBETEXTURE);
                if(upTex_ptr!=NULL)
                    cubeTex->get_texture_blocks()->force_append(upTex_ptr);
                AWDBitmapTexture * downTex_ptr=ExportBitmapTexture(downTex, NULL, cubeTex->get_tex_type(), FORCUBETEXTURE);
                if(downTex_ptr!=NULL)
                    cubeTex->get_texture_blocks()->force_append(downTex_ptr);
                AWDBitmapTexture * frontTex_ptr=ExportBitmapTexture(frontTex, NULL, cubeTex->get_tex_type(), FORCUBETEXTURE);
                if(frontTex_ptr!=NULL)
                    cubeTex->get_texture_blocks()->force_append(frontTex_ptr);
                AWDBitmapTexture * backTex_ptr=ExportBitmapTexture(backTex, NULL, cubeTex->get_tex_type(), FORCUBETEXTURE);
                if(backTex_ptr!=NULL)
                    cubeTex->get_texture_blocks()->force_append(backTex_ptr);
                if(cubeTex->get_texture_blocks()->get_num_blocks()!=6){
                    AWDMessageBlock * newWarning = new AWDMessageBlock(cubetexName_ptr, "AWDCubeTexture has not 6 valid materials applied. CubeTexture will not be exported");
                    awd->get_message_blocks()->append(newWarning);
                    cubeTex->make_invalide();
                    return NULL;
                }
            }
        }
        else{
            if (strlen(replaceURL_ptr)==0){
                AWDMessageBlock * newWarning = new AWDMessageBlock(cubetexName_ptr, "AWDCubeTexture should be replaced by ATFCubeTexture, but no url is set. CubeTexture will not be exported");
                awd->get_message_blocks()->append(newWarning);
                cubeTex->make_invalide();
            }
            else{
                //TODO: implement ATFCubetexture in AWD
                AWDMessageBlock * newWarning = new AWDMessageBlock(cubetexName_ptr, "AWDCubeTexture should be replaced by ATFCubeTexture, but ATFCubeTextures are not supported in the workflow yet.");
                awd->get_message_blocks()->append(newWarning);
                cubeTex->make_invalide();
            }
        }
        free(cubetexName_ptr);
        if (replaceURL_ptr!=NULL)
            free(replaceURL_ptr);
    }
    return cubeTex;
}