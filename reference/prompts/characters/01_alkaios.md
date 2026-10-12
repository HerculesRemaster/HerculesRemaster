# Alkaios — nhân vật chính

## Concept

**Vai trò:** hero điều khiển trong cả 10 màn. Á thần trẻ, con của một vị thần chưa lộ danh tính. Được Chiron nuôi dạy
trên núi Pelion. Mục tiêu là thu hồi quả táo vàng của Eris trước khi bất hòa nhấn chìm cả Hy Lạp.

**Tính cách:** mạnh quá mức kiểm soát, hay phá đồ vô tình rồi lúng túng xin lỗi. Hài hước tự giễu, tò mò, tốt bụng,
liều lĩnh. Không ngây thơ "đi tìm nơi thuộc về". Động lực của cậu là chứng minh mình kiểm soát được sức mạnh.

**Hình khối và silhouette:**
- Tam giác ngược vừa phải: vai rộng, eo khỏe, chân chắc. Tỉ lệ khoảng 7 đầu, không kiểu cartoon ngực khổng lồ.
- Điểm nhận diện số 1 là **mũ da sư tử**: hàm trên và bờm sư tử trùm trán như mũ giáp, hai tai dựng. Ở mọi góc nhìn,
  silhouette đầu phải khác hẳn đầu người trần.
- Hai chân trước của sư tử buộc chéo trước ngực tạo chữ X. Phần da phía sau rủ xuống lưng tới bắp chân, mép tua lông
  bay khi chạy (thay cho áo choàng vải).

**Khuôn mặt:** mặt hơi vuông nhưng trẻ, gò má cao, mũi thẳng kiểu tượng Hy Lạp, lông mày đen dày, mắt nâu hổ phách,
sẹo nhỏ ở cằm trái. Nụ cười lệch một bên. Tóc đen tết thành 6–8 bím ngắn buộc dây da, thò ra dưới mũ sư tử.

**Trang phục từ trên xuống:**
1. Da sư tử Nemea màu vàng nâu, bờm sẫm hơn ở mép, lông có texture thật.
2. Giáp ngực linothorax bằng đồng phủ patina xanh ngọc, viền đồng sáng ở các cạnh mòn, giữa ngực dập nổi đầu sư tử.
3. Chiton ngắn màu đỏ terracotta lộ ở vai và gấu.
4. Pteryges: 2 tầng dải da nâu sẫm có đinh đồng.
5. Thắt lưng da rộng, khóa đồng hình móng vuốt.
6. Bao cổ tay bằng da quấn dây.
7. Xà cạp đồng ôm ống chân, khắc meander mảnh.
8. Sandal da buộc dây tới bắp chân.

**Vũ khí:**
- **Chùy gỗ ô liu** (vũ khí chính, đánh mạnh, dùng cho MAGS): thân gỗ xoắn tự nhiên, 3 đai đồng, đầu chùy có mấu.
  Khi không dùng thì đeo chéo sau lưng.
- **Xiphos** (đòn nhanh): kiếm ngắn lưỡi hình lá bằng đồng, cán quấn da, đeo bên hông trái.
- **Đấm** tay không: đốt tay quấn băng da.

**Power-up (boon):** khi có boon, vũ khí đổi hình và hào quang đổi màu:
| Boon | Hình | Màu |
| --- | --- | --- |
| Giáo của Ares | giáo dài, mũi đỏ rực | đỏ máu |
| Cung của Artemis | cung bạc hình trăng lưỡi liềm | bạc ánh trăng |
| Khiên Aegis | khiên tròn đồng gắn mặt Gorgon cách điệu, bất tử tạm thời | xanh lục ngọc |
| Búa của Hephaestus | búa rèn, mỗi cú nện tóe lửa | cam lò rèn |

**Bảng màu:** vàng bờm `#C8963E`, đồng patina `#4F8A7B`, da nâu sẫm `#5A3A28`, đỏ terracotta `#A4442B`, đồng sáng `#C9A15A`.

**Cấm (tránh giống Hercules của Disney):** áo choàng vải xanh dương, băng đô, tóc nâu đỏ gợn sóng, tunic nâu trơn,
biểu tượng tia sét, mặt chữ điền cằm chẻ kiểu cartoon.

**Animation set:** idle, idle chờ lâu (phủi bụi da sư tử, vươn vai), chạy, phanh gấp, quay đầu, nhảy lên/đỉnh/rơi/tiếp
đất, đấm, chém xiphos, đòn chùy, power punch (tích 3 cấp và ra đòn), MAGS (nện chùy từ trên không, sóng chấn),
treo mép, leo, đu xà, bị đánh, bị hất văng, chết, hồi sinh ở bàn thờ lửa, cưỡi Aithon (bay, né, tấn công), nhặt đồ,
kích hoạt boon, ăn mừng cuối màn.

---

## Prompt

### A1 — Turnaround (anchor, sinh đầu tiên)

```text
Character turnaround model sheet of an original game hero. {{ALKAIOS}}
Layout: four full-body views in a row (front, three-quarter, side profile, back), neutral A-pose, same scale, plain warm light-grey studio background, soft even lighting with subtle rim light, small close-up insets of the lion-pelt hood, the chest emblem, the club and the xiphos along the bottom. No text labels.
{{STYLE}}
{{AVOID}}
```

### A2 — Biểu cảm

```text
Facial expression sheet of an original game hero, head and shoulders only, 3x3 grid on a plain light-grey background. {{ALKAIOS}}
Expressions: confident grin, determined battle shout, embarrassed after breaking something, surprised, laughing, exhausted, angry, sad, focused charging a punch. Keep the lion-pelt hood, braided black hair, chin scar and face shape identical in every cell. No text.
{{STYLE}}
{{AVOID}}
```

### A3 — Bộ tư thế hành động

```text
Action pose sheet of an original 2.5D platformer hero, side view, 8 full-body poses on a plain light-grey background, consistent scale: running, mid-jump, falling, olive-wood club overhead smash, quick xiphos slash, charging a glowing power punch with amber energy, mid-air downward ground slam with a shockwave, hanging from a ledge with one hand. {{ALKAIOS}}
The lion pelt's back flap and paws react dynamically to motion. No text.
{{STYLE}}
{{AVOID}}
```

### A4 — Chi tiết đạo cụ

```text
Prop design sheet on a plain light-grey background: the Nemean lion pelt hood seen front, side and back; the verdigris bronze linothorax cuirass with embossed lion head; the heavy twisted olive-wood club with three bronze bands; the short leaf-bladed bronze xiphos with a leather-wrapped grip and scabbard; bronze greaves with a fine meander engraving; laced leather sandals; a claw-shaped bronze belt buckle. Realistic material rendering: worn bronze with green patina, polished edges, real fur, aged leather, grained wood. No text.
{{STYLE}}
{{AVOID}}
```

### A5 — Bốn boon (power-up)

```text
Four-panel character sheet, each panel the same hero in a dynamic attack pose holding a different divine weapon with its own glow: (1) a long war spear with a blazing blood-red tip and red aura, (2) a silver crescent-moon bow shooting a moonlight arrow with silver aura, (3) a round bronze shield bearing a stylized gorgon face with emerald-green protective aura, (4) a heavy forge hammer throwing orange sparks with orange aura. {{ALKAIOS}}
Plain dark-grey background with a subtle vignette, no text.
{{STYLE}}
{{AVOID}}
```

### A6 — Key art (poster dọc)

```text
Vertical key art poster for an original Greek-myth action platformer. {{ALKAIOS}}
He stands on a broken marble ledge at the top of a mountain at sunrise, club resting on his shoulder, lion-pelt hood blowing in the wind, looking toward the camera with a confident grin. Behind him a sea of golden clouds, distant temples on floating cliffs, the dark silhouette of a tall goddess holding a glowing golden apple in the storm clouds above. At the bottom, the title "ALKAIOS" in carved gold stone letters with a meander underline.
{{STYLE}}
{{AVOID}}
```
