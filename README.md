# ResourcePack Reference (RPR)

> **当前为第一发布版本，分离工具集暂不完整，后续更新会添加相关工具并删除此句。**

- [README 中文](README.md)
- [README English](README_EN.md)
- [README TokiPona](README_TP.md)

![icon](resource/icon.png) 《我的世界》基岩版资源包静态分析工具集。

`rpr.exe` 为主程序（实际执行端），其余程序为分离工具集，转换参数来调用 `rpr.exe`。

部分工具依赖 `.rpi`（ResourcePack Index）文件，推荐先运行 `rpi_index` 生成索引文件。

## 下载与使用

- **直接下载**: [rpref.zip](https://github.com/SusumaarT/RP-Reference/releases/latest/download/rpref.zip)
- **使用方法**:

    ```batch
    rpr -mcp              :: 启动 MCP 服务器
    rpr -tool=<tool_name> :: 运行指定工具
    rpr -info=<tool_name> :: 获取指定工具的信息
    rpr -tools            :: 列出所有可用的工具
    ```

## 工具列表

> 该列表的工具名称为 MCP 服务器工具名称，不一定与分离工具名称相同。

- `ac_to_mmd`：将 **动画控制器** 转换为 Mermaid 状态图或流程图，便于查看状态机逻辑。
- `rpi_index`：扫描资源包目录并生成 **.rpi** 索引文件。
- `rpi_find`：在 **.rpi** 文件中根据关键字查找组件或文件。
- `sn_map`：为实体或资源建立短名映射，帮助理解命名和引用关系。
- `bone_coverage`：检查几何体与动画控制器之间的骨骼集合关系。
- `geo_tree`：展示几何体的层级结构和父子关系。
- `br_extract`：解压 **.brarchive** 归档文件，提取其中的资源内容。
- `br_zip`：压缩资源内容为 **.brarchive** 归档文件。
- `br_merge`：合并多个 **.brarchive** 文件，生成新的归档。
- `br_auto`：自动执行资源包的提取或归档操作。
- `dir_compo_rename`：按目录内 JSON 键值条件批量重命名组件目录，支持自定义命名格式。
- `rpi_compo_rename`：基于资源包索引批量重命名组件，适合统一命名规范。
- `unmerge`：将已合并的 JSON 对象或数组拆分回多个组件片段。
- `merged`：将多个组件 JSON 文件按指定路径合并为一个整体结构。
- `mat_forest`：分析材质树结构和依赖关系。

> 无参数运行分离程序以获取使用方法。

## 从源码构建

> 仅在 Windows 环境下测试项目（通过 CMD 调用 `mingw32-make`）。

1. 克隆本仓库。
2. 确保您已安装 GNU GCC 编译器套件
    >（最好拥有 Tiny C Compiler (TCC) 编译器，也可在 `Makefile.win` 中修改为gcc）。
3. 打开 CMD 命令提示符，进入项目根目录。
4. 运行根目录已有的脚本：
    1. `clean.bat`: 清理构建目录。
    2. `debug.bat`: 以调试模式编译。
    3. `release.bat`: 以发布模式编译。
5. 在 `bin` 目录下获取可执行文件。

## 许可协议

本项目采用 GNU AGPL v3.0 协议开源，详细信息请参阅 [LICENSE](LICENSE) 文件。
