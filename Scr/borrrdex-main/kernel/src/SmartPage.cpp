#include "SmartPage.h"
#include "paging/PageFrameAllocator.h"

SmartPage::SmartPage(int pageCount) 
    :_count(pageCount)
{
    _start = PageFrameAllocator::SharedAllocator()->RequestPages((uint64_t)pageCount);
}

SmartPage::~SmartPage() {
    PageFrameAllocator::SharedAllocator()->FreePages(_start, (uint64_t)_count);
}

SmartPage::operator void *() {
    return _start;
}