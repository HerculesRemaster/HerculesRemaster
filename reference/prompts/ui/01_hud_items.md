# HUD và vật phẩm

## Concept HUD

Ngôn ngữ thiết kế: **gốm hình đen trên nền đất nung** + viền meander đồng. Đọc rõ ở 1080p trên TV.

| Thành phần | Vị trí | Thiết kế | Trạng thái |
| --- | --- | --- | --- |
| Huy chương khiên | Trên trái | Khiên hoplon tròn bằng đất nung, đầu sư tử vẽ kiểu hình đen, viền đồng | Rung khi bị đánh |
| Số mạng | Dưới huy chương | Các vạch khắc (tally) trên tấm đồng nhỏ, tối đa 9 vạch | Vạch mới sáng lên khi thêm mạng |
| Thanh máu | Phải huy chương | Cành ô liu bằng đồng dài, mỗi lá là một đơn vị máu, có ngọn lửa vàng nhỏ | Lá mất máu tắt lửa và xám; còn ít máu thì lửa chập chờn đỏ |
| Ô vũ khí/boon | Trên phải | Mảnh gốm vuông vát góc, hình đen của vũ khí, viền meander | Thời gian boon còn lại là vết nứt lan dần |
| Thanh power | Dưới phải | Tấm đồng nhỏ có 3 viên đá hổ phách | Sáng dần theo cấp tích lực |
| Thanh máu boss | Dưới giữa | Bia cẩm thạch nứt, lõi đỏ đồng phát sáng, icon boss kiểu hình đen | Nứt thêm khi boss mất máu |
| Hint | Góc dưới trái | Cú Glaux bay vào, cầm cuộn giấy | Xuất hiện 3 giây |

## Concept vật phẩm

| Vật phẩm | Chức năng | Thiết kế |
| --- | --- | --- |
| Obol | Điểm, tính % | Đồng xu đồng nhỏ, in hình cú Glaux |
| Vòng nguyệt quế | Điểm hiếm | Vòng lá nguyệt quế vàng phát sáng |
| Sao Pleiades | Bộ sưu tập màn (7 sao) | Ngôi sao 7 cánh bạc xanh, quỹ đạo hạt sáng |
| Bình ambrosia | Hồi máu | Bình amphora nhỏ, mật vàng óng phát sáng, nút sáp đỏ |
| Táo vàng nhỏ | Hồi đầy máu (L06) | Táo vàng có lá xanh |
| Tượng terracotta | Thêm mạng | Tượng nhỏ Alkaios đội da sư tử bằng đất nung đỏ |
| Ostrakon | Bí mật | Mảnh gốm đen-trên-đỏ có khắc chữ Hy Lạp; 4 mảnh ghép thành một bình |
| Lông vũ Zephyr | Tăng tốc | Lông vũ trắng xanh nhạt, có vệt gió xoáy |
| Bàn thờ lửa | Checkpoint | Bàn thờ đá nhỏ, lửa bùng lên khi chạm |
| Boon (4 loại) | Power-up | Quả cầu ánh sáng: Ares đỏ có giáo, Artemis bạc có cung, Athena (Aegis) xanh lục có khiên, Hephaestus cam có búa |

---

## Prompt

### U1 — HUD sheet (anchor)

```text
Game UI asset sheet on a flat dark charcoal background, clean separated elements, front-facing, no perspective, high detail, ancient Greek black-figure pottery style on terracotta with bronze trim: (1) a round terracotta hoplon-shield medallion with a black-figure stylized lion head and a bronze rim; (2) a small bronze plate with nine carved tally marks, some glowing; (3) a long health bar shaped like a bronze olive branch with twelve leaves, the first eight leaves holding small glowing golden flames and the last four dimmed grey; (4) four beveled square terracotta pottery-shard icons with bronze meander borders, each with a black-figure silhouette: an olive-wood club, a war spear, a crescent bow, a round shield; (5) a pottery-shard icon with a spreading crack; (6) a small bronze plaque with three amber gems, two glowing; (7) a long cracked marble tablet boss health bar with a glowing deep red-bronze fill. No text, no clouds, no lightning bolts, no wings.
```

### U2 — Item sheet

```text
Game item asset sheet on a flat dark charcoal background, clean separated 3D-rendered pickups, each with a soft glow, consistent lighting, no text: a small bronze obol coin stamped with an owl; a glowing golden laurel wreath; a silver-blue seven-pointed star with orbiting light particles; a small amphora filled with glowing golden honey-nectar and a red wax seal; a small golden apple with a green leaf; a red terracotta figurine of a young hero wearing a lion-pelt hood; four black-on-red pottery shards with Greek letters that fit together into a vase, shown apart and assembled; a white-and-pale-blue feather with a swirl of wind; a small stone altar with a burst of flame; four glowing orbs: red with a spear, silver with a bow, emerald with a shield, orange with a hammer.
STYLE: high-end stylized 3D CGI, feature-film animation quality characters with appealing heroic proportions, highly detailed near-photoreal environments, cinematic volumetric lighting with god rays and strong rim light separating characters from the background, rich saturated color grading with warm/cool contrast, wet reflective stone floors, atmospheric particles (embers, mist, sparks, water spray), weathered ancient Greek architecture with meander and anthemion ornament, crisp sharp focus, epic scale, Unreal Engine 5 cinematic look.
```

### U3 — HUD trên một màn mẫu

```text
4:3 landscape gameplay screenshot of an original 2.5D action platformer showing the full HUD clearly, side view, simple bright level: the hero runs across a sunny marble terrace with olive trees and a few bronze coins. Alkaios: a young Greek demigod hero of about 18, athletic heroic build with broad shoulders and strong arms (powerful but not exaggerated), tanned Mediterranean skin, short black hair worn in several small braids tied with leather cords, thick dark eyebrows, warm amber-brown eyes, a small scar on his chin, confident lopsided grin; he wears the golden-tawny pelt of the Nemean lion as a hooded cloak, the lion's upper jaw and mane framing his forehead like a helmet and the two front paws knotted across his chest; a verdigris patina-green bronze linothorax cuirass with an embossed lion-head emblem; a terracotta-red short chiton under the armor; dark brown leather pteryges strips with bronze studs; bronze greaves; laced leather sandals; a heavy olive-wood club with bronze bands; a short leaf-bladed bronze xiphos at his hip.
HUD overlay (clean, readable, game UI): top-left a round terracotta hoplon-shield medallion painted in black-figure pottery style with a stylized lion head, three small carved tally marks under it for lives; to its right a long health bar shaped like a bronze olive branch whose leaves glow with small golden flames (some leaves dimmed to show lost health); top-right a beveled square terracotta pottery-shard icon showing the current weapon as a black-figure silhouette (an olive-wood club), framed by a thin bronze meander border; bottom-right a small bronze plaque with three glowing amber gems as the power charge meter. The HUD has no clouds, no lightning bolts and no wings.
Small owl hint icon at the bottom-left holding a scroll.
STYLE: high-end stylized 3D CGI, feature-film animation quality characters with appealing heroic proportions, highly detailed near-photoreal environments, cinematic volumetric lighting with god rays and strong rim light separating characters from the background, rich saturated color grading with warm/cool contrast, wet reflective stone floors, atmospheric particles (embers, mist, sparks, water spray), weathered ancient Greek architecture with meander and anthemion ornament, crisp sharp focus, epic scale, Unreal Engine 5 cinematic look.
Avoid: blue cape, red or brown headband, auburn or ginger hair on the hero, a short goat-legged trainer, a white winged horse with a blue mane, a villain with blue flaming hair, a chorus of five singing women, lightning bolt ornaments, silver cloud-scroll frames, any logos, watermarks, or text other than the text requested.
```
