#include <kot-ui++/component.h>

#include <kot-ui++/pictures/picture.h>

#include <kot/uisd/srvs/storage.h>

namespace Ui {

    Component* Picturebox(char* Path, ImageType Type, ImageStyle Style) {
        file_t* imageFile = fopen(Path, "rb");

        if(imageFile == NULL)
            return NULL;

        fseek(imageFile, 0, SEEK_END);
        size_t imageFileSize = ftell(imageFile);
        fseek(imageFile, 0, SEEK_SET);

        TGAHeader_t* image = (TGAHeader_t*) malloc(imageFileSize);
        fread(image, imageFileSize, 1, imageFile);

        uint16_t Width = image->Width;
        uint16_t Height = image->Height;

        if(Width <= 0 || Height <= 0) { free(image); fclose(imageFile); return NULL; }

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

        Component* picture = new Component({ .Width = Style.Width, .Height = Style.Height });

        switch(Type)
        {
            case ImageType::_TGA:
                uint32_t* Pixels = TGARead(image);

                for(uint16_t y = 0; y < Height; y++) {
                    for(uint16_t x = 0; x < Width; x++) {
                        PutPixel(picture->GetFramebuffer(), picture->GetStyle()->X+x, picture->GetStyle()->Y+y, Pixels[x + y*Width]);
                    }
                }

                free(Pixels);

                break;
        }

        free(image);
        fclose(imageFile);
        
        return picture;
    }

}