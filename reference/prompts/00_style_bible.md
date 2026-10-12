# Style bible

Đây là nguồn gốc của mọi đoạn văn lặp lại trong các prompt. Khi sửa style, sửa ở đây trước rồi cập nhật các file
khác cho khớp. Mỗi prompt trong bộ này đã chứa sẵn các khối cần thiết, nên copy một prompt là đủ.

## 1. Style rút ra từ `reference/CGI/`

Đặc điểm chung của 19 ảnh CGI tham chiếu (chỉ lấy style, không lấy nhân vật):

| Yếu tố | Mô tả |
| --- | --- |
| Render | 3D CGI stylized chất lượng phim hoạt hình chiếu rạp cho nhân vật; môi trường chi tiết gần photoreal |
| Ánh sáng | Volumetric, tia nắng (god rays), rim light mạnh tách nhân vật khỏi nền, nguồn sáng thực tế (đuốc, lửa, pha lê) |
| Màu | Bão hòa cao, tương phản nóng/lạnh: vàng cam hoàng hôn, xanh lam băng, xanh ngọc âm phủ, tím huyền bí |
| Chất liệu | Đá vôi và cẩm thạch nứt, đồng mạ vàng, nền đá ướt phản chiếu, rêu, dây leo, nước, băng |
| Hiệu ứng | Tàn lửa, sương, bụi nước, tia lửa, hạt sáng, nước bắn dưới chân |
| Trang trí | Meander (chìa khóa Hy Lạp), anthemion (hoa cọ), cột Doric/Corinthian, tượng đá, phù điêu |
| Bố cục gameplay | Camera ngang (side view), hơi cao, tỉ lệ 4:3; hero ở 1/3 dưới, chiếm ~1/5 chiều cao; boss lớn chiếm nửa trên |
| Bố cục HUD | Góc trên trái: chân dung + máu; góc trên phải: vũ khí; thỉnh thoảng có thanh phụ ở dưới phải |
| Loading screen | Khung viền anthemion đen trên nền đất nung, chữ vàng 3D nứt, nhân vật đứng hai bên |

## 2. Khối STYLE (dán vào mọi prompt gameplay và môi trường)

```text
STYLE: high-end stylized 3D CGI, feature-film animation quality characters with appealing heroic proportions, highly detailed near-photoreal environments, cinematic volumetric lighting with god rays and strong rim light separating characters from the background, rich saturated color grading with warm/cool contrast, wet reflective stone floors, atmospheric particles (embers, mist, sparks, water spray), weathered ancient Greek architecture with meander and anthemion ornament, crisp sharp focus, epic scale, Unreal Engine 5 cinematic look.
```

## 3. Khối HUD (dán vào prompt screenshot gameplay)

```text
HUD overlay (clean, readable, game UI): top-left a round terracotta hoplon-shield medallion painted in black-figure pottery style with a stylized lion head, three small carved tally marks under it for lives; to its right a long health bar shaped like a bronze olive branch whose leaves glow with small golden flames (some leaves dimmed to show lost health); top-right a beveled square terracotta pottery-shard icon showing the current weapon as a black-figure silhouette (an olive-wood club), framed by a thin bronze meander border; bottom-right a small bronze plaque with three glowing amber gems as the power charge meter. The HUD has no clouds, no lightning bolts and no wings.
```

## 3b. Khối BOSSBAR (thêm vào prompt boss fight)

```text
Boss health bar at the bottom center: a long horizontal cracked marble tablet with a glowing deep red-bronze fill and a small black-figure pottery icon of the boss at its left end.
```

## 4. Khối AVOID (dán vào cuối mọi prompt)

```text
Avoid: blue cape, red or brown headband, auburn or ginger hair on the hero, a short goat-legged trainer, a white winged horse with a blue mane, a villain with blue flaming hair, a chorus of five singing women, lightning bolt ornaments, silver cloud-scroll frames, any logos, watermarks, or text other than the text requested.
```

## 5. Character lock (mô tả cố định, dán nguyên văn)

Dùng đúng câu chữ dưới đây mỗi khi nhân vật xuất hiện để model vẽ nhất quán.

**ALKAIOS**

```text
Alkaios: a young Greek demigod hero of about 18, athletic heroic build with broad shoulders and strong arms (powerful but not exaggerated), tanned Mediterranean skin, short black hair worn in several small braids tied with leather cords, thick dark eyebrows, warm amber-brown eyes, a small scar on his chin, confident lopsided grin; he wears the golden-tawny pelt of the Nemean lion as a hooded cloak, the lion's upper jaw and mane framing his forehead like a helmet and the two front paws knotted across his chest; a verdigris patina-green bronze linothorax cuirass with an embossed lion-head emblem; a terracotta-red short chiton under the armor; dark brown leather pteryges strips with bronze studs; bronze greaves; laced leather sandals; a heavy olive-wood club with bronze bands; a short leaf-bladed bronze xiphos at his hip.
```

**CHIRON**

```text
Chiron: a wise elderly centaur, tall and lean; the human torso of a fit old man with weathered olive skin, a long silver beard braided at the tip, a bald crown with long silver hair at the back bound by a bronze ring, calm grey-green eyes with deep laugh lines; a rough indigo wool mantle over one shoulder, a leather satchel of herbs and scrolls, a horn recurve bow and a quiver; his horse body is dapple-grey with darker charcoal legs, feathered fetlocks and a long grey tail.
```

**AITHON**

```text
Aithon: a proud winged stallion with a coppery red-bronze coat, mane and tail made of glowing ember-orange flame, large eagle-like wings with golden-brown feathers tipped in dark bronze, bronze-shod hooves, amber eyes with a haughty cat-like expression, a lean athletic racehorse build, a bronze bridle with small lion-head cheek pieces.
```

**IOLAUS**

```text
Iolaus: a wiry 14-year-old Greek boy, light-brown curly hair, freckles, a bright mischievous grin, a saffron-yellow short chiton with a rope belt, a leather satchel, leather sandals, carrying a burning resin torch.
```

**GLAUX**

```text
Glaux: a small silver-grey little owl with huge golden eyes, feathers patterned in fine silver like an ancient Athenian coin, a faint soft silver glow.
```

**ERIS**

```text
Eris, goddess of discord: tall, slender, elegant and unsettling; warm porcelain-white skin with fine glowing gold cracks like kintsugi on her cheeks and hands; long straight black hair with a violet sheen held by a thorned gold diadem; narrow golden eyes; a black peplos with bronze-gold geometric meander trim and a high stiff collar; the hem dissolves into drifting dark violet smoke and golden sparks; she holds a glowing golden apple.
```

## 6. Bảng màu

| Nhóm | Màu chính |
| --- | --- |
| Alkaios | vàng bờm sư tử `#C8963E`, đồng patina `#4F8A7B`, da sẫm `#5A3A28`, đỏ terracotta `#A4442B` |
| Chiron | xám đốm `#8E8E88`, chàm `#2F3A66`, bạc `#C9CCD1` |
| Aithon | đồng đỏ `#9C4A2A`, lửa than `#FF8A2B`, nâu vàng lông chim `#A87A3D` |
| Eris | đen `#141018`, tím than `#4A2A5E`, vàng kim `#E2B34A`, trắng sứ `#EDE6DC` |
| HUD | đất nung `#B5562E`, đen gốm `#1B1512`, đồng `#B07A3A`, lửa vàng `#FFC24A` |

## 7. Tên game (tạm)

Prompt dùng chữ **ALKAIOS** cho title và loading. Cần tra cứu trademark trước khi chốt.
