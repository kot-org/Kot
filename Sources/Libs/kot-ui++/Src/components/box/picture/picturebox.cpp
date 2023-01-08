#include <kot-ui++/component.h>

#include <kot-ui++/pictures/picture.h>

namespace Ui {

    Component* Picturebox(char* path, ImageType type, ImageStyle style) {
        Component* picture = new Component({ .Width = style.Width, .Height = style.Height });

        switch(type)
        {
            case ImageType::_TGA:
                TGA* tgaImg = new TGA(path, picture);

                break;
        }
        
        return picture;
    }

}