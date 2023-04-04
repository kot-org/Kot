#include <kot/heap.h>
#include <kot/types.h>
#include <kot/memory.h>

#include <kot++/printf.h>

namespace std{
    template <typename T>
    class vector{
        public:
            vector(){
                Size = 0;
                Capacity = 4;
                Data = new T[Capacity];
                Lock = 0;
            }

            vector(const vector& Other){
                Size = Other.Size;
                Capacity = Other.Capacity;
                Data = new T[Capacity];
                for(uint64_t i = 0; i < Size; ++i){
                    Data[i] = Other.Data[i];
                }
            }

            ~vector(){
                delete Data;
            }

            uint64_t push(const T& Item){
                atomicAcquire(&Lock, 0);
                if(Size == Capacity){
                    expand();
                }
                uint64_t Index = Size;
                Data[Index] = Item;
                Size++;
                atomicUnlock(&Lock, 0);
                return Index;
            }

            void remove(uint64_t Index){
                atomicAcquire(&Lock, 0);
                if(Index < 0 || Index >= Size){
                    return;
                }
                for(uint64_t i = Index; i < Size - 1; ++i){
                    Data[i] = Data[i + 1];
                }
                Size--;
                atomicUnlock(&Lock, 0);
            }

            void set(uint64_t Index, const T& Item){
                atomicAcquire(&Lock, 0);
                if(Index < 0 || Index >= Size){
                    return;
                }
                Data[Index] = Item;
                atomicUnlock(&Lock, 0);
            }

            void expand(uint64_t len = 4){
                Capacity += len;
                T* NewData = new T[Capacity];
                memcpy((uintptr_t)NewData, (uintptr_t)Data, sizeof(T) * Size);
                delete Data;
                Data = NewData;
            }

            T& get(uint64_t Index){
                atomicAcquire(&Lock, 0);
                T& DataOutput = Data[Index];
                atomicUnlock(&Lock, 0);
                return DataOutput;
            }

            T& operator[](uint64_t Index){
                return get(Index);
            }

            void clear(){
                Size = 0;
            }

            vector<T> clone(){
                return vector<T>(*this);
            }

            size64_t size() const{
                return Size;
            }

            size64_t capacity() const{
                return Capacity;
            }

        private:
            T* Data;
            size64_t Size;
            size64_t Capacity;
            uint64_t Lock;
    };

}
