# 地图格子数据

`MapData` 是格子值对象，包含 `MapCost`（0–255）、调用方定义的 `TerrainType`（uint32，0 表示未指定）和独立的障碍标志。地形 ID 目前只是分类信息，不自动推导移动能力，也没有预设草地、水面等枚举。不同单位的通行策略不在此提交中实现。

```cpp
FindTheWay::MapData water(30, 2, false);
map.SetMapData(x, y, water, FindTheWay::MapType::TERRAIN);
auto cell = map.GetMapData(x, y, FindTheWay::MapType::MAP);
if (!cell.IsObstacle()) {
    const auto cost = cell.GetCost();
}
```

GetMapData 返回副本；修改副本后需调用 SetMapData 写回，这样仍会经过更新/缓存失效流程。needUpdate=false 延续旧接口约定：调用方负责显式 UpdateMap。MAP 是派生层，SetMapData 写接口只允许 TERRAIN/DYNAMIC。

## 分层及行为变化

- TERRAIN/DYNAMIC 分别保存完整格子值。派生 MAP 的地形 ID 来自 TERRAIN，代价为两层代价之和（饱和到 255），障碍标志为两层障碍标志的 OR。DYNAMIC 的地形 ID 可单独读取和保存，但不覆盖 TERRAIN 分类。
- **代价不再决定障碍：0 代价可以阻挡，255 代价也可以通行。** 寻路、拐角检查、直线/范围计算和调试地图均读取障碍标志。
- 与旧实现相比，两层非障碍代价相加达到 255 时不再自动变成障碍。这是本提交明确引入的行为变化。
- ForceFind 正常返回时恢复整个起终点格子，包括地形 ID 和障碍标志；不增加异常恢复策略。
- 分层合成由 FindTheWayClass 管理，单层 MapData 不负责合成；元数据暂只保存、不参与规则。可破坏障碍放在动态层，清除动态障碍后恢复基础地形；基础地形自身的障碍不会被动态层清除。
- MapData 不提供隐式数值转换，也不能把对象数组当字节块 memset/memcpy/Base64。

## 事件适配边界

MapData 类不包含 FromLegacy、ToLegacy 或 IsLegacy 等兼容接口。核心 GetMap/SetMap/ClearMap/IterateMap 始终使用完整 MapData，GetPointCost 返回真实代价。

`FindTheWayFusion.h` 定义派生兼容类 `FindTheWayFusion`，仅在 Windows 事件绑定和 Android JNI 使用。拥有对象的声明和创建都使用兼容类；原有数值调用保持不变。事件边界中：事件输入 255 转为显式障碍，0–254 转为通行。兼容 SetMap 保留已有地形 ID；兼容 ClearMap 重置整层。数值事件输出仍将 255 保留给障碍，可通行且真实代价为 255 的格子只在此边界投影为 254。该投影不会进入核心存储、遍历和代价计算。

Android 同时提供完整 GetMapData/SetMapData，Java 的 MapData 是不可变快照，terrainType 使用 long 表达完整无符号 uint32。新 JNI 操作只用于封送；没有新增 Fusion ACE，也没有复制寻路规则到 Java。

## 存档

- 始终导出当前格式 `FTW2:width:height:terrain:dynamic`，不再探测能否降级保存。旧运行库不能读取该版本。
- 存档输入仍支持旧格式 `width:height:terrain:dynamic`；由地图读取入口把旧字节 255 转为显式障碍，载入后只保留当前模型，下一次导出即升级为 FTW2。
- FTW2 每个源层的每格编码为六个字节：cost、obstacle（0/1）、terrainType 的四字节小端序。没有直接序列化 C++ 对象布局、填充或 bool 表示。
- 导入先校验字段、长度和标志，再替换地图。格式错误保持原地图；不承诺分配失败恢复。

测试和 CMake 位于 Android_Gradle/tests/helpers。MapDataTest 覆盖独立障碍、完整存档、缓存更新、ForceFind、以及 100 张包含高代价和独立障碍的地图与 Dijkstra 对照；JNI 测试覆盖无符号地形 ID 和版本化往返。EventAdapterTest 验证数值边界、分类保留，以及核心读取/遍历无损。Windows/Android 真实工具链仍需作者验证。
