# mint_poly 测试

从仓库根目录运行。默认命令运行正确性测试，`--stress` 在此基础上加测数据规模上界，`--bench` 单独测耗时。

```sh
python3 tests/mint_poly/run.py
python3 tests/mint_poly/run.py --stress
python3 tests/mint_poly/run.py --bench
```

## 正确性与边界

| 接口 | 检查 |
|---|---|
| 缺省源辅助函数、`mint`、`C`、`binom` | 流输出、模运算整数对拍、帕斯卡三角及有符号输入边界 |
| `DIF`、`DIT`、一维和二维卷积及转置 | 直接 DFT、往返恢复、朴素卷积和滑窗内积 |
| 多项式算术、`der`、`ints`、`inv`、`exp`、`ln`、`sqrt`、`qpow`、`division` | 朴素乘法和系数递推、商余重构、短除数、前导零与大指数 |
| `multipoint_eval`、`composite`、`composite_inverse` | 重复点、单点、长度不等、Horner 法及正反复合 |
| 一元与二元 `BostanMori`、`BostanMoriT` | 独立系数展开、转置逐项对拍、朴素线性递推及 `i64` 最大下标 |
| `dc`、`dln`、`dexp`、`dinv` | 稠密与稀疏输入，每次 `push` 后同朴素参考逐项比较 |

正确性测试使用固定随机种子，覆盖长度为 1 以及 64、128、256 前后的分支。长度为 1 的 NTT 和短除数保留独立回归入口。测试启用 UBSan 和 libstdc++ 容器边界断言，任一用例失败都会返回非零。上界测试额外检查达到 `S = 2^21` 点 NTT 的卷积、长度 `S/2` 的求逆和指数、长度 `S` 的常数除法、`S-1` 次在线卷积，以及分母次数为 `S/2-1` 的一元系数提取。

## 性能对比

将旧版源文件传给 `--baseline` 可做同机对比。两版均需提供当前基准调用的接口。

```sh
python3 tests/mint_poly/run.py --bench --baseline /path/to/old/mint_poly.cpp --rounds 5
```

所有模式均使用 `-std=gnu++20 -O2 -Wall -Wextra -Wconversion -Werror`。基准报告毫秒中位数，带基线时交替运行两版并报告百分比变化。每轮核对项目集合、输出形状和全系数哈希。计时包括运算和结果构造，输入生成、全局预处理与哈希在计时区间外。NTT 往返用 `S` 点，卷积、求逆、幂级数除法和指数用 `S/2` 长度，其余接口使用文件内固定的代表性规模。耗时供同机比较。

## 新增功能时

1. 在 `check.cpp` 中添加独立参考值或朴素对拍，并从测试入口调用。补上可以触发新增分支的最小输入、算法切换点和有关的上界。
2. 在 `bench.cpp` 中登记新增运算的代表性规模，涉及预处理上界时在 `limits()` 中添加可核对的用例。
3. 先跑默认测试，涉及边界或算法修改时跑 `--stress`，再跑 `--bench` 检查输出与耗时。有兼容的修改前源码时加 `--baseline`。
