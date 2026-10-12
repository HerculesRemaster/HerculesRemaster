# Bộ prompt concept: ALKAIOS (IP gốc, thần thoại Hy Lạp)

Bộ prompt để sinh concept art bằng **GPT Image 2.0** cho game platformer 2.5D IP gốc. Nhân vật, HUD và level theo
brief "Brief thiết kế nhân vật & HUD — Greek Myth Platformer (IP gốc)". Style lấy từ các ảnh trong
`reference/CGI/` và đã được mô tả thành chữ trong từng prompt.

> **Không đính kèm ảnh trong `reference/CGI/` làm image reference.** Hầu hết các ảnh đó chứa nhân vật Disney
> (Hercules, Meg, Phil, Muses, Pegasus) và một ảnh chứa logo Disney Interactive. Nếu đính kèm, model sẽ vẽ lại
> các thiết kế đó. Style (ánh sáng, chất liệu, bố cục, độ chi tiết) đã được chép thành chữ trong
> [00_style_bible.md](00_style_bible.md).

## Cấu trúc

| Thư mục / file | Nội dung |
| --- | --- |
| [00_style_bible.md](00_style_bible.md) | Style chung, bảng màu, HUD chuẩn, danh sách cần tránh, khóa mô tả nhân vật (character lock) |
| [characters/](characters/) | Nhân vật chính, người thầy, thú cưỡi, đồng hành, phản diện: concept chi tiết + prompt turnaround, biểu cảm, tư thế |
| [bosses/](bosses/) | 8 boss: concept chi tiết + prompt sheet và cảnh chiến đấu |
| [enemies/](enemies/) | Kẻ thù thường và NPC |
| [levels/](levels/) | 10 level: concept, set piece, prompt screenshot gameplay, môi trường, loading screen |
| [ui/](ui/) | HUD, vật phẩm, title, menu, loading, Level Complete, Game Over, bản đồ |

## Thứ tự sinh (quan trọng để giữ nhất quán)

1. **Anchor nhân vật chính:** [characters/01_alkaios.md](characters/01_alkaios.md), prompt **A1 Turnaround**. Sinh tới khi
   đạt, chỉnh tay nếu cần. Lưu thành `anchors/alkaios_sheet.png`.
2. **Anchor HUD:** [ui/01_hud_items.md](ui/01_hud_items.md), prompt **U1**. Lưu thành `anchors/hud_sheet.png`.
3. **Anchor nhân vật phụ:** Chiron, Aithon, Iolaus, Glaux, Eris (prompt turnaround của từng file).
4. **Boss và kẻ thù:** sheet trước, rồi tới cảnh chiến đấu.
5. **Level:** screenshot gameplay. **Đính kèm** `anchors/alkaios_sheet.png` (và `anchors/hud_sheet.png` nếu có HUD)
   rồi giữ nguyên câu *"Use the attached character sheet as the exact design of Alkaios"* trong prompt.
6. **UI và màn hình:** title, loading, Level Complete.

Chỉ dùng **ảnh anchor mới đã duyệt** làm reference. Không dùng ảnh trong `reference/CGI/`.

## Kích thước

- Ảnh CGI gốc có tỉ lệ **4:3** (1448×1086). Prompt gameplay ghi "4:3 landscape".
- Với GPT Image, chọn khổ ngang (landscape) gần nhất mà công cụ hỗ trợ (ví dụ 1536×1024), rồi crop về 4:3 nếu cần.
- Character sheet và boss sheet: khổ ngang. Poster và key art dọc: khổ dọc.

## Cách dùng một prompt

1. Mở file `.md`, copy toàn bộ khối ```` ```text ```` của prompt muốn sinh (đã có style, HUD và mục cần tránh bên trong).
2. Đính kèm anchor nếu prompt yêu cầu.
3. Sinh 2–4 biến thể, chọn bản tốt nhất, lưu vào `reference/concepts/<thư mục>/<mã prompt>_<số>.png`
   (ví dụ `reference/concepts/levels/L03-A_01.png`).
4. Chấm theo checklist bên dưới.

## Checklist duyệt (mọi ảnh)

- [ ] Alkaios: khoác **da sư tử** trùm đầu, **tóc đen tết**, giáp **đồng patina xanh**. Không áo choàng xanh dương, không băng đô, không tóc nâu đỏ
- [ ] Không satyr trainer, không ngựa trắng bờm xanh, không thần lửa xanh, không nhóm 5 nữ thần hát
- [ ] HUD: khiên đất nung + cành ô liu lửa + mảnh gốm. Không mây xoắn bạc, không tia sét, không cánh vàng
- [ ] Không chữ "Hercules", không logo, không watermark
- [ ] Silhouette nhân vật đọc rõ trên nền; hero chiếm khoảng 1/5 chiều cao màn hình trong ảnh gameplay
- [ ] Ảnh concept được chọn sẽ được họa sĩ chỉnh tay trước khi chuyển sang 3D

## Danh sách prompt

| Mã | File | Nội dung |
| --- | --- | --- |
| A1–A6 | characters/01_alkaios.md | Turnaround, biểu cảm, bộ tư thế hành động, chi tiết đạo cụ, power-up, key art |
| C1–C3 | characters/02_chiron.md | Turnaround, biểu cảm, cảnh dạy Alkaios |
| P1–P3 | characters/03_aithon.md | Turnaround, tư thế bay, Alkaios cưỡi Aithon |
| I1–I2 | characters/04_iolaus.md | Turnaround, biểu cảm |
| G1 | characters/05_glaux.md | Sheet cú Glaux |
| E1–E3 | characters/06_eris.md | Turnaround, biểu cảm, chân dung phản diện |
| D1 | characters/07_deimos_phobos.md | Sheet hai tay sai |
| B1–B16 | bosses/*.md | Sheet + cảnh chiến đấu cho 8 boss |
| N1–N4 | enemies/01_enemies_npcs.md | Sheet kẻ thù và NPC |
| L01–L10 | levels/*.md | Màn chơi, mỗi màn 3–5 prompt |
| U1–U8 | ui/*.md | HUD, vật phẩm, title, loading, Level Complete, Game Over, bản đồ, bộ sưu tập |
