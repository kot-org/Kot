#include <kot-ui++/component.h>

#include <kot-ui++/pictures/picture.h>

#include <kot/uisd/srvs/storage.h>

namespace Ui {
    void PictureboxDraw(Picturebox_t* Picturebox){
        switch(Picturebox->Type){
            case PictureboxType::_TGA:{
                switch(Picturebox->Style.Fit){
                    case PictureboxFit::PICTUREFIT:
                    {   
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        TGA_t* ImageResize = TGAResize(ImageRead, Picturebox->Cpnt->GetFramebuffer()->Width, Picturebox->Cpnt->GetFramebuffer()->Height, true);

                        ImageResize->x = Picturebox->Cpnt->GetFramebuffer()->Width / 2 - ImageResize->Width / 2;
                        ImageResize->y = Picturebox->Cpnt->GetFramebuffer()->Height / 2 - ImageResize->Height / 2;

                        TGADraw(Picturebox->Cpnt->GetFramebuffer(), ImageResize);

                        free(ImageResize);
                        free(ImageRead);
                        break;
                    }

                    case PictureboxFit::PICTUREFILL:
                    {   
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        uint16_t _Width = ((TGAHeader_t*)Picturebox->Image)->Width, _Height = ((TGAHeader_t*)Picturebox->Image)->Height;

                        // permet de faire en sorte que l'image dépasse la taille du monitor
                        if(_Width > _Height) {
                            _Width = NULL;
                            _Height = Picturebox->Cpnt->GetFramebuffer()->Height;
                        } else{
                            _Width = Picturebox->Cpnt->GetFramebuffer()->Width;
                            _Height = NULL;
                        }
                        
                        TGA_t* ImageResize = TGAResize(ImageRead, _Width, _Height, true);

                        uint16_t x = (ImageResize->Width - Picturebox->Cpnt->GetFramebuffer()->Width) / 2;
                        uint16_t y = (ImageResize->Height - Picturebox->Cpnt->GetFramebuffer()->Height) / 2;

                        TGA_t* ImageCrop = TGACrop(ImageResize, Picturebox->Cpnt->GetFramebuffer()->Width, Picturebox->Cpnt->GetFramebuffer()->Height, x, y);
                        
                        TGADraw(Picturebox->Cpnt->GetFramebuffer(), ImageCrop);

                        free(ImageResize);
                        free(ImageCrop);
                        free(ImageRead);
                        break;
                    }

                    case PictureboxFit::PICTURECENTER:
                    {
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        uint16_t x = 0, y = 0,
                            _Width = ((TGAHeader_t*)Picturebox->Image)->Width, _Height = ((TGAHeader_t*)Picturebox->Image)->Height;

                        if(_Width > Picturebox->Cpnt->GetFramebuffer()->Width) {
                            x = (_Width - Picturebox->Cpnt->GetFramebuffer()->Width) / 2;
                            _Width = Picturebox->Cpnt->GetFramebuffer()->Width;
                        }
                        if(_Height > Picturebox->Cpnt->GetFramebuffer()->Height) {
                            y = (_Height - Picturebox->Cpnt->GetFramebuffer()->Height) / 2;
                            _Height = Picturebox->Cpnt->GetFramebuffer()->Height;
                        }

                        // si l'image dépasse le monitor alors on crop
                        if(((TGAHeader_t*)Picturebox->Image)->Width > Picturebox->Cpnt->GetFramebuffer()->Width || ((TGAHeader_t*)Picturebox->Image)->Height > Picturebox->Cpnt->GetFramebuffer()->Height) {
                            TGA_t* ImageCrop = TGACrop(ImageRead, _Width, _Height, x, y);

                            ImageCrop->x = Picturebox->Cpnt->GetFramebuffer()->Width / 2 - ImageCrop->Width / 2;
                            ImageCrop->y = Picturebox->Cpnt->GetFramebuffer()->Height / 2 - ImageCrop->Height / 2;

                            TGADraw(Picturebox->Cpnt->GetFramebuffer(), ImageCrop);
                            
                            free(ImageCrop);
                        } else {
                            ((TGAHeader_t*)Picturebox->Image)->x = Picturebox->Cpnt->GetFramebuffer()->Width / 2 - ((TGAHeader_t*)Picturebox->Image)->Width / 2;
                            ((TGAHeader_t*)Picturebox->Image)->y = Picturebox->Cpnt->GetFramebuffer()->Height / 2 - ((TGAHeader_t*)Picturebox->Image)->Height / 2;
                            
                            TGADraw(Picturebox->Cpnt->GetFramebuffer(), ImageRead);
                        }

                        free(ImageRead);

                        break;
                    }

                    case PictureboxFit::PICTURESTRETCH:
                    {   
                        TGA_t* ImageRead = TGARead((TGAHeader_t*)Picturebox->Image);
                        TGA_t* ImageResize = TGAResize(ImageRead, Picturebox->Cpnt->GetFramebuffer()->Width, Picturebox->Cpnt->GetFramebuffer()->Height);
                        TGADraw(Picturebox->Cpnt->GetFramebuffer(), ImageResize);

                        free(ImageResize);
                        free(ImageRead);
                        break;
                    }

                    default:
                        break;
                }
            }
        }
    }
    
    void PictureboxUpdate(Component* Cpnt){
        Picturebox_t* Picturebox = (Picturebox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Picturebox->Style.Width = Cpnt->GetStyle()->Width;
            Picturebox->Style.Height = Cpnt->GetStyle()->Height;

            PictureboxDraw(Picturebox);
            Cpnt->IsFramebufferUpdate = false;
        }else if(Picturebox->IsDrawUpdate){
            PictureboxDraw(Picturebox);
            Picturebox->IsDrawUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Picturebox->Style.Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Picturebox->Style.Position.y};
        if(Cpnt->GetStyle()->IsVisible){
            BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Cpnt->Style->Position.x, Cpnt->Style->Position.y);
            // Do not use event
        }
    }

    void PictureboxMouseEvent(class Component* Cpnt, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        if(IsHover){
            Picturebox_t* Picturebox = (Picturebox_t*)Cpnt->ExternalData;
            if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                    Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                }
            }
            Cpnt->UiCtx->FocusCpnt = Picturebox->Cpnt;
        }
    }

    Picturebox_t* Picturebox(char* Path, PictureboxType Type, PictureboxStyle_t Style, UiContext* ParentUiContex) {
        file_t* ImageFile = fopen(Path, "rb");

        if(ImageFile == NULL)
            return NULL;

        fseek(ImageFile, 0, SEEK_END);
        size_t ImageFileSize = ftell(ImageFile);
        fseek(ImageFile, 0, SEEK_SET);

        uintptr_t Image = malloc(ImageFileSize);
        fread(Image, ImageFileSize, 1, ImageFile);

        uint16_t Width;
        uint16_t Height;
        switch(Type){
            case PictureboxType::_TGA:
                Width = ((TGAHeader_t*)Image)->Width;
                Height = ((TGAHeader_t*)Image)->Height;
                break;
            default:
                free(Image); 
                fclose(ImageFile);
                return NULL;
                break;
        }

        if(Width <= 0 || Height <= 0) { 
            free(Image); 
            fclose(ImageFile); 
            return NULL; 
        }

        /* Si le component n'a pas de taille alors la taille est égal à celle de l'image
            Si il a une taille alors on resize l'image
         */
        if(Style.Width == NULL)
            Style.Width = Width;
        else
            Width = Style.Width;
        
        if(Style.Height == NULL)
            Style.Height = Height;
        else
            Height = Style.Height; 

        Picturebox_t* Picturebox = (Picturebox_t*)malloc(sizeof(Picturebox_t));
        Picturebox->Type = Type;
        Picturebox->Image = Image;
        memcpy(&Picturebox->Style, &Style, sizeof(PictureboxStyle_t));
        Picturebox->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height, .IsVisible = Style.IsVisible, .Position = {.x = Style.Position.x, .y = Style.Position.y}}, PictureboxUpdate, PictureboxMouseEvent, (uintptr_t)Picturebox, ParentUiContex, true);

        fclose(ImageFile);
        
        return Picturebox;
    }

}