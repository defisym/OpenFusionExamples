#ifndef FINDTHEWAY_CORE_H
#define FINDTHEWAY_CORE_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* C ABI for map lifecycle, cell costs, and last-path queries. All calls execute on the owning Fusion
 * thread. Handles are opaque, never reused, and remain invalid after release.
 * Outputs are written only on success. No exceptions cross this boundary. */
typedef uint64_t FTWMap;
typedef int32_t FTWStatus;
enum { FTW_OK=0, FTW_INVALID_ARGUMENT=1, FTW_INVALID_HANDLE=2,
       FTW_OUT_OF_MEMORY=3, FTW_CORE_ERROR=4, FTW_BUFFER_TOO_SMALL=5 };
enum { FTW_MAP=0, FTW_TERRAIN=1, FTW_DYNAMIC=2 };
typedef struct FTWCoord { int32_t x; int32_t y; } FTWCoord;
enum { FTW_FIND_DIAGONAL=1, FTW_FIND_CHECK_CORNERS=2, FTW_FIND_FORCE=4 };
/* A successful search with no route returns OK and count zero. Path coordinates
 * include both endpoints. Copy the result before the next search/map mutation.
 * A null buffer with zero capacity queries the count. BUFFER_TOO_SMALL writes
 * required count only, leaving the coordinate buffer untouched. */
FTWStatus ftw_map_find_path(FTWMap map, FTWCoord start, FTWCoord goal,
                            uint32_t flags, uint64_t *out_count);
FTWStatus ftw_map_copy_path(FTWMap map, FTWCoord *buffer, uint64_t capacity,
                            uint64_t *out_count);
uint32_t ftw_abi_version(void);
FTWStatus ftw_map_create(int32_t width, int32_t height, FTWMap *out_map);
/* Releasing an already released handle (including zero) succeeds. */
FTWStatus ftw_map_release(FTWMap map);
/* Serialization is the core's ASCII map format, with explicit byte length.
 * Export count excludes any terminator; no NUL is appended. Query with NULL/0.
 * Too-small buffers are untouched and return the required byte count. */
FTWStatus ftw_map_create_serialized(const char *bytes, uint64_t length, FTWMap *out_map);
FTWStatus ftw_map_export(FTWMap map, char *buffer, uint64_t capacity, uint64_t *out_length);
/* Isometric width/height are packed by the bridge, not by the caller.
 * Signed offsets are translated at the C boundary to avoid LLP64/LP64 differences. */
FTWStatus ftw_map_set_grid(FTWMap map, int32_t isometric, int32_t width,
                           int32_t height, int32_t offset_x, int32_t offset_y);
FTWStatus ftw_map_convert_coord(FTWMap map, FTWCoord input, int32_t to_grid,
                                FTWCoord *out_coord);
FTWStatus ftw_map_set_cost(FTWMap map, int32_t x, int32_t y, int32_t cost, int32_t layer);
FTWStatus ftw_map_get_cost(FTWMap map, int32_t x, int32_t y, int32_t layer, int32_t *out_cost);
/* ABI 2: Windows algorithm coverage. Lists are copied, not retained. UTF-8
 * path names are NUL terminated; NULL/empty selects the last path. */
typedef struct FTWLists {
    const FTWCoord *ally, *enemy, *zoc;
    uint64_t ally_count, enemy_count, zoc_count;
} FTWLists;
typedef struct FTWAreaOptions {
    FTWCoord start;
    int32_t line, attack, range, start_range, ignore, all_range, attack_range;
} FTWAreaOptions;
typedef struct FTWAreaPoint { FTWCoord coord; uint64_t ring; } FTWAreaPoint;
FTWStatus ftw_map_set_lists(FTWMap, const FTWLists *);
FTWStatus ftw_map_find_ex(FTWMap, FTWCoord, FTWCoord, uint32_t flags,
                         uint32_t ignore, const char *save_name, uint64_t *count);
FTWStatus ftw_map_named_path(FTWMap, const char *name, FTWCoord *, uint64_t, uint64_t *);
FTWStatus ftw_map_area(FTWMap, const FTWAreaOptions *, uint64_t *rings, uint64_t *attack_start);
FTWStatus ftw_map_copy_area(FTWMap, FTWAreaPoint *, uint64_t, uint64_t *);
FTWStatus ftw_map_line_range(FTWMap, FTWCoord, int32_t direction, int32_t range,
                            uint32_t ignore, int32_t attack, int32_t *result);
FTWStatus ftw_generate_zoc(FTWCoord, FTWCoord *, uint64_t, uint64_t *);
FTWStatus ftw_map_aoe(FTWMap, FTWCoord, uint32_t direction, uint32_t type,
                     uint32_t ignore, FTWCoord *, uint64_t, uint64_t *);
FTWStatus ftw_map_stash(FTWMap, int32_t enable);
FTWStatus ftw_map_clear_costs(FTWMap, int32_t cost, int32_t layer);
/* Desktop GetMap semantics: outside-map coordinates sample as obstacle (255),
 * while the original strict get_cost API retains argument validation. */
FTWStatus ftw_map_sample_cost(FTWMap, int32_t x, int32_t y, int32_t layer, int32_t *cost);
FTWStatus ftw_map_dimensions(FTWMap, int32_t *width, int32_t *height);
FTWStatus ftw_map_display(FTWMap, int32_t layer, int32_t show_path,
                         const char *name, char *, uint64_t, uint64_t *);
/* Poll lazy merged-map updates without invoking Runtime code inside C++. */
FTWStatus ftw_map_take_updates(FTWMap, uint64_t *count);
/* Callback is borrowed, synchronous on the owner thread. Do not throw through it.
 * The core retains its map across callbacks; replacement/release is permitted. */
typedef void (*FTWUpdateCallback)(void *context, FTWMap map);
FTWStatus ftw_map_set_callback(FTWMap, FTWUpdateCallback, void *context);
#ifdef __cplusplus
}
#endif
#endif
