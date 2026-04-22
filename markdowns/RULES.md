// 這裡只寫我們程式規範部份
# 規則

1. 函數回傳狀態 `correct -> 0 error -> 非0之數` 通常為 -1 如果回傳非-1 則需要寫註解解釋回傳的數字所代表的意思
    為減少記憶體用量回傳數值盡可能使用 `int8_t` 或者`enum`作為我們的回傳形式 若使用enum則註解寫在enum上

2. 檔案架構最上層：`README.md` `.ino` ~/{include/`headers相關` ,srcs/`.cpp相關檔案` } markdown類型放`~/markdowns/`

3. 適當使用struct enum 其定義統一放在 .h 內 

4. 統一使用一個.clang-format 格式化避免格式混雜

套用方法：
```sh
clang-format --version
clang-format -i include/*.h srcs/*.cpp
```
沒有clang-format 的話因為我們OS都是arch所以
```sh
sudo pacman -Syu
sudo pacman -S clang
```
