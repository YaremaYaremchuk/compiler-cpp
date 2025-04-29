#pragma once

class ArenaAllocator {

public:

    inline explicit ArenaAllocator(size_t size): cur_size(size){

        cur_buffer = static_cast<std::byte *>(malloc(size));

        cur_offset = cur_buffer;




    }


    template<typename T>
    inline T* alloc() {

        void* offset = cur_offset;
        cur_offset += sizeof(T);
        return static_cast<T*>(offset);


    }
    inline ArenaAllocator operator=(const ArenaAllocator &other) =delete;

    inline ArenaAllocator(const ArenaAllocator& other) = delete;


    inline ~ArenaAllocator() {
        free(cur_buffer);
    }
private:

    size_t cur_size;

    std::byte* cur_buffer;

    std::byte* cur_offset;
};