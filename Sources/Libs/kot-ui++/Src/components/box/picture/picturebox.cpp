#include <kot-ui++/component.h>

#include <kot-ui++/pictures/picture.h>

#include <kot/uisd/srvs/storage.h>

namespace Ui {
    void PictureboxDraw(Component* Cpnt){
        // Picturebox_t* Picturebox = (Picturebox_t*)Cpnt->ExternalData;

        // switch(Picturebox->Type){
        //     case PictureboxType::_TGA:
        //         TGA_t* TGAImage = TGARead((Ui::TGAHeader_t *)Picturebox->Image);

        //         for(uint16_t y = 0; y < TGAImage->Height; y++) {
        //             for(uint16_t x = 0; x < TGAImage->Width; x++) {
        //                 PutPixel(Picturebox->Cpnt->GetFramebuffer(), Picturebox->Cpnt->GetStyle()->x+x, Picturebox->Cpnt->GetStyle()->y+y, TGAImage->Pixels[x + y*TGAImage->Width]);
        //             }
        //         }

        //         free(TGAImage);

        //         break;
        // }
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
        Picturebox->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height }, PictureboxDraw, NULL, Picturebox, ParentUiContex, true);

        fclose(ImageFile);
        
        return Picturebox;
    }

}