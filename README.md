# 医院诊疗信息管理系统（Qt 实验三）

一个基于 Qt SQL 与 Model/View 结构的简单医院诊疗系统用户信息管理程序。
开发者：林苗苗（2024414300214）。

## 功能

- 登录检测：区分用户名不存在、密码错误、数据库未连接三种情况；支持注册新用户
- QStackedWidget 界面切换：登录页 → 欢迎页 → 患者管理 / 操作记录 / 科室管理 / 医生管理
- 患者信息：TableView 列表显示、按编号/姓名/身份证/手机号查询、新增、修改、删除
- 性别列使用自定义代理（QStyledItemDelegate + QComboBox）编辑，数据库仍存整数
- 删除前有确认对话框；新增/修改检查必填字段与重复身份证；修改后重新查询刷新视图
- 操作记录写入 History 表，可在界面中查询
- 选做：科室管理、医生管理

## 编译

本项目用 CMake + Ninja，Qt 6.11.2 MinGW 64 位：

```bash
cmake -G Ninja -B build -S . \
  -DCMAKE_PREFIX_PATH="D:/Qt/6.11.2/mingw_64" \
  -DCMAKE_CXX_COMPILER="D:/Qt/Tools/mingw1310_64/bin/g++.exe"
cmake --build build
```

数据库文件 `hospital.db` 会在程序首次运行时自动创建在 exe 同目录，
表结构见 `sql/schema.sql`。演示账号：`admin / 123456`。

## 自动演示

程序支持 `--demo` 参数自动走完登录、查询、新增、修改、删除、查看操作记录等流程，
用于实验截图和视频取证：

```bash
HospitalManagement.exe --demo --dir <输出目录> --interval 1500 --shots
```

普通运行不加参数即可。
