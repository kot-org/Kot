#include <kot-ui++/component.h>

using namespace Ui;

namespace UiLayout {

    void CalculateAlignment(Component* parent, uint32_t index);

    void CalculatePosition(Component* parent, uint32_t index);

    void CalculateFlex(Component* parent, uint32_t index);
    void RecalculateFlexChilds(Component* parent);

    void CalculateGrid(Component* parent, uint32_t index);
}