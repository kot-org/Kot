#include <kot-ui++/component.h>

using namespace Ui;

namespace UiLayout {

    void calculateAlignment(Component* parent, uint32_t index);

    void calculatePosition(Component* parent, uint32_t index);

    void calculateFlex(Component* parent, uint32_t index);
    void recalculateFlexChilds(Component* parent);

}