#include <kot-ui++/component.h>

#include <kot-ui++/pictures/picture.h>

namespace Ui {

    Component* Picturebox(char* path, ImageType type, ImageStyle style) {
        file_t* imageFile = fopen(path, "r");

        if(imageFile == NULL)
            return NULL;

        Component* picture = new Component({ .Width = style.Width, .Height = style.Height });

        switch(type)
        {
            case ImageType::_TGA:
                TGA* tgaImage = new TGA(imageFile);

                break;
        }
        
        return picture;
    }

}