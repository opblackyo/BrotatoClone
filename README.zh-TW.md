# BrotatoClone

[English](README.md) | [繁體中文](README.zh-TW.md)

這是一款以 Brotato 為靈感、使用 C++17 並基於 PTSD framework 製作的倖存者類遊戲。

本專案從 PTSD template 出發，擴充成一個可遊玩的 20 波流程，包含角色成長、商店、Boss 戰與 UI 覆蓋介面。

## 框架與技術

- 語言：C++17
- 建置系統：CMake，最低版本 3.16
- 框架：PTSD，Practical Tools for Simple Design
- 繪圖、輸入、音效：由 PTSD 相關依賴提供

PTSD 參考：
https://github.com/ntut-open-source-club/practical-tools-for-simple-design

PTSD template：
https://github.com/ntut-open-source-club/ptsd-template

## 目前遊戲內容

- 主畫面與完整遊戲流程
- 20 波關卡結構
- 一般波次為 30 秒
- 第 20 波為最終 Boss，並有階段轉換
- 自動攻擊戰鬥，包含 4 種武器：
   - Pistol
   - Shotgun
   - SMG
   - Knife
- 武器階級與合成系統：
   - 兩把相同的一階武器合成二階
   - 兩把相同的二階武器合成三階
   - 高階武器會提升傷害並降低冷卻時間
- 敵人種類與進程：
   - Chaser
   - Bruiser
   - Spitter
   - Guardian
   - Bomber
   - Boss
- 波次結束後的成長流程：
   - 屬性選擇，依表現獲得 1 或 2 次選擇
   - 商店階段，可重抽、購買、販售武器
- 經濟與續戰系統：
   - 材料球，也就是金錢掉落
   - 撿取範圍與波後自動吸回材料
   - 樹木破壞與水果治療
   - 生命竊取、生命回復、護甲、閃避、暴擊等成長屬性
- 隨機地圖主題：
   - Dirt
   - Forest
   - Volcano
   - Dreamy lands
   - Boneyard
   - Darklands
- UI 與回饋：
   - HUD，顯示血量、金錢、波次、時間
   - Boss 血條
   - 暫停選單，顯示完整屬性與已持有道具
   - 傷害數字、受擊/死亡效果、槍口火光
   - 射擊、受擊、購買、重抽、波次轉換音效

## 遊戲系統

### 戰鬥

玩家手動移動，武器會自動攻擊範圍內的敵人或樹木。預設情況下武器優先攻擊敵人。按住 T 時，若樹木在攻擊範圍內，武器會優先攻擊樹木。

敵人死亡後會掉落材料球。材料球被撿起時會同時提供金錢與 XP。材料球在玩家撿取範圍內會自動吸向玩家；波次結束時，場上剩餘材料會先自動吸回，才進入屬性選擇與商店階段。

### 武器與合成

武器目前有三個階級：

- Tier I：基礎武器
- Tier II：由兩把相同 Tier I 武器合成
- Tier III：由兩把相同 Tier II 武器合成

購買武器後會自動嘗試與已持有的同名同階武器合成。若六個武器欄位已滿，只要購買的武器能立刻與現有武器合成，仍然可以購買。

目前階級提升會直接強化武器：

- 每階傷害約提升 50%
- 每階冷卻時間約縮短 20%

武器階級在商店與武器列表中有顏色區分：

- Tier I：白色
- Tier II：綠色
- Tier III：藍色

### 商店與成長

每個一般波次結束後，玩家會獲得屬性升級選項。若該波表現較好，可能獲得兩次屬性選擇。

接著會進入商店，玩家可以：

- 購買道具卡
- 購買武器卡
- 重抽商店選項
- 販售已持有武器
- 開始下一波

目前尚未實作存檔/讀檔，每次遊玩都是獨立的一局。

## 波次設計

遊戲共有 20 波。一般波次為 30 秒，第 20 波為最終 Boss 戰。

敵人生成使用 wave profile 控制，而不是單純隨機混合。每個 profile 定義敵人總數、場上存活上限、生成速度、批次生成數量與敵人種類權重。

目前波次節奏：

- Waves 1-3：前期小怪海
   - 主要是 Chaser
   - 大量低血敵人
   - 少量 Bruiser
- Waves 4-6：第一次混合壓力
   - Chaser 與 Bruiser 持續出現
   - Spitter 開始加入
   - 玩家需要開始注意走位
- Waves 7-9：遠程壓力
   - Spitter 權重提高
   - 玩家需要一邊清近戰敵人，一邊閃避投射物
- Waves 10-12：坦怪與爆炸敵人
   - Guardian 與 Bomber 加入
   - 開始測試玩家輸出與站位
- Waves 13-16：完整混合波
   - Chaser、Bruiser、Spitter、Guardian、Bomber 都會出現
   - 生成速度與場上敵人上限提高
- Waves 17-19：Boss 前壓力測試
   - 生成更快
   - Guardian 與 Bomber 比例提高
   - 用來測試玩家 build 是否足以進入 Boss 戰
- Wave 20：Boss
   - 單一 Boss 敵人
   - Boss 有多方向遠程攻擊，低血量時進入第二階段

## 地圖系統

每波開始時，場地背景會隨機選擇。系統使用接近 Brotato 的單色調地面與少量地表裝飾，而不是完整的大型地圖背景圖。

每種地圖主題包含：

- 大型單色背景圖
- 微弱光影與雜訊
- 少量不重疊的地面裝飾圖

地面裝飾只影響視覺，不會阻擋移動。

## 操作方式

### 主畫面

- Enter 或 Space：開始遊戲

### 波次中

- W/A/S/D 或方向鍵：移動
- 按住 T：優先攻擊樹木
- Esc：開啟暫停選單

### 屬性選擇

- 1/2/3：選擇升級卡
- 滑鼠左鍵：選擇升級卡

### 商店

- 1/2/3/4：購買道具卡
- 滑鼠左鍵：購買道具卡
- 0 或 Enter：進入下一波
- 滑鼠左鍵點 REROLL：重抽商店
- 滑鼠左鍵點已持有武器欄位：販售該武器

### 暫停

- Esc：繼續遊戲
- 滑鼠左鍵點 RESUME：繼續遊戲
- 滑鼠左鍵點 QUIT：立即離開

### 結果畫面

- R：重新開始
- Esc：離開

## 建置與執行

注意：目前專案只支援 Debug build。

Release 目前會刻意失敗，因為 CMake 中相對資源路徑處理仍在施工中。

### 需求

- Git
- CMake 3.16+
- C++ 編譯器，MSVC、Clang 或 GCC
- 支援 OpenGL 的執行環境
- Ninja，可選

### Clone

```bash
git clone <YOUR_REPO_URL> --recursive
cd BrotatoClone
```

### Configure，建議方式

不使用 Ninja：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

使用 Ninja：

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

### Build

```bash
cmake --build build --config Debug -j
```

### Run

從專案根目錄執行，PowerShell：

```powershell
.\build\BrotatoClone.exe
```

或從 build 資料夾執行：

```powershell
cd build
.\BrotatoClone.exe
```

## Windows 批次檔

本 repository 目前包含：

- configure.bat
- build.bat

這些批次檔目前是特定機器路徑，指向 D 槽的 CLion bundled CMake 與 Ninja。若要在其他電腦使用，請先修改批次檔中的路徑。

## 專案結構

- src
   - 核心遊戲實作
   - 場景流程、實體、武器、UI 系統
- include
   - 遊戲系統的公開 header
- Resources
   - 執行時資源，包含字型、圖片、音效
- PTSD
   - 由 CMake FetchContent 取得的 framework 原始碼
- CMakeLists.txt
   - 建置入口與依賴整合

## 遊戲流程

1. 主畫面
2. 波次戰鬥
3. 屬性選擇
4. 商店
5. 下一波
6. 第 20 波 Boss
7. 勝利或遊戲結束

## 已知限制

- 僅支援 Debug build，Release 資源路徑尚未處理
- 尚未實作存檔/讀檔
- 尚未串接自動化測試
- 批次檔需要修改本機路徑後才適合在其他電腦使用

## 疑難排解

### 遊戲啟動後因文字建立錯誤退出

若 log 中出現類似訊息：

- Failed to create text
- Text has zero width

請確認：

- Build type 是 Debug
- Resources 資料夾存在且未被移動
- Resources/fonts/Inter.ttf 存在
- 修改路徑後有重新 configure

### CMake 第一次 configure 失敗

第一次 configure 會透過 FetchContent 下載 PTSD 與第三方依賴。請確認網路可用後重新執行 configure。

## Credits

- PTSD framework by NTUT Open-Source Club
- Brotato-inspired game design and pacing references

## License

MIT. See LICENSE for details.
