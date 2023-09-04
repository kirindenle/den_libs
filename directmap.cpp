struct Direct_Map {
    void *id;
    int index;
};

// Указатель на это должен быть в структуре
struct Direct_Map_Helper {
    char *attributes = nullptr;
    int attributes_size = 0;
    int *starts = nullptr;
    // TODO SPEED можно при обращении в мапу всегда переставлять её на начало чтобы не проходить весь массив при каждом обращении к той же мапе
    void **ids = nullptr;
    int attributes_count = 0;
};

static void direct_map_optimize(Direct_Map_Helper *helper, int i) {
    if (i != 0) {
        int tmp_s = helper->starts[0];
        void *tmp_id = helper->ids[0];
        helper->starts[0] = helper->starts[i];
        helper->ids[0]    = helper->ids[i];
        helper->starts[i] = tmp_s;
        helper->ids[i] = tmp_id;
    }
}

int direct_map_find_start(Direct_Map *map, Direct_Map_Helper *helper) {
    for (int i = 0; i < helper->attributes_count; ++i) {
        if (helper->ids[i] == map->id) {
            direct_map_optimize(helper, i);
            return helper->starts[0];
        }
    }
    return -1;
}

// TODO владение?
// TODO типобезопасность?
// TODO alignas
// TODO возвращать значение, а значит звать все чудо-конструкторы C++
template<typename T>
T *direct_map_get(Direct_Map *map, Direct_Map_Helper *helper) {
    if (!helper) return nullptr;

    int start = direct_map_find_start(map, helper);
    if (start == -1) return nullptr;

    //assert(start + sizeof(T) <= helper->attributes_size);
    return (T*)helper->attributes[start];
}

// tests
#ifdef TESTS
#include <stdio.h>
struct Test {
    int val;
    Direct_Map_Helper* maps;
};

int main() {
    Test t;
    Direct_Map map;
    map.id = &map;
    int *mapped_val = direct_map_get<int>(&map, t.maps);
    printf("%p\n", mapped_val);
    return 0;
}
#endif
