#pragma once

#include <kot/sys.h>

#include <kot-ui++/component.h>

namespace Ui {

    class FlexLayout {
            
        public:

            enum FlexDirection {
                HORIZONTAL,
                VERTICAL,
            };

            Component* cpnt;

            FlexLayout(FlexDirection direction);

        private:

    };

}