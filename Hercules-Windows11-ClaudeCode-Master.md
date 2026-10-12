# Chạy Disney Hercules trên Windows 11 64 bit bằng Claude Code

Tài liệu thực hiện và prompt giao việc · Ngày kiểm tra nguồn 11 tháng 10 năm 2026

## 1 Kết luận và phạm vi

Có một hướng khả thi để chạy bản Windows của Disney’s Hercules trên Windows 11 x64 mà không cài máy ảo hoặc Windows XP: xử lý bộ cài cũ, giữ chương trình game x86, rồi chỉ thêm lớp tương thích đồ họa nếu thử chạy thực tế cho thấy cần thiết. Claude Code có thể chuẩn bị, chẩn đoán và tự động hóa quy trình này. Claude Code không phải phần mềm giả lập và không cần chạy trong lúc bạn chơi.

Điều kiện quan trọng: phải kiểm tra chính các file trong `Disneys_Hercules_Win_ISO_EN`. Tên bộ tải về không xác nhận kiến trúc chương trình, phiên bản InstallShield, mức đầy đủ của dữ liệu hay khả năng chạy trên máy của bạn. Chưa có ISO hoặc kết quả thử trên máy Windows của bạn trong tài liệu này; các đoạn mã bên dưới là mẫu để Claude Code kiểm tra và áp dụng tại máy đích, không phải bản vá đã được chạy thử với bản game đó.

Windows x64 có WOW64 để chạy ứng dụng x86; không cần chuyển game thành ứng dụng x64. Bộ cài 16-bit và game 32-bit là hai thành phần khác nhau. Microsoft xác nhận Windows x64 không chạy trực tiếp thành phần 16-bit. [1][2]

### Hiểu đúng yêu cầu không cần gì khác

| Yêu cầu | Khả năng đáp ứng |
|---|---|
| Không máy ảo, không cài Windows cũ, không DOSBox | Là mục tiêu của quy trình chính |
| Không cài launcher hay dịch vụ chạy nền cho game | Có thể, nếu bản game và dữ liệu cho phép |
| Sau thiết lập chỉ bấm shortcut để chơi, không mở Claude | Có thể thiết kế như vậy |
| Không thêm bất kỳ file từ bên ngoài | Chỉ có thể nếu dữ liệu hiện có đủ để cài thủ công và chạy bằng Windows sẵn có |
| Cho phép tải công cụ dùng một lần và DLL cạnh game | Là nhánh thực dụng nhất nếu bộ cài hoặc đồ họa không tương thích |
| Một EXE duy nhất, không DLL, không dữ liệu ngoài, không mount đĩa | Không phải kết quả có thể hứa trước cho bản ISO này |

**Đề xuất:** thử nhánh không thêm phụ thuộc trước; khi có lỗi cụ thể, dùng bộ cài thay thế hoặc wrapper đặt trong thư mục game. Nếu bạn tuyệt đối không cho phép file hỗ trợ, Claude phải dừng ở giới hạn đã xác định thay vì tự mở rộng phạm vi.

## 2 Bằng chứng online và điều còn phải xác minh

| Nội dung | Mức xác nhận | Ý nghĩa |
|---|---|---|
| Windows x64 chạy ứng dụng 32-bit qua WOW64 | Tài liệu Microsoft [1] | Không cần port game sang x64 chỉ để chạy trên OS x64 |
| Thành phần 16-bit không được hỗ trợ trực tiếp | Tài liệu Microsoft [2] | Compatibility Mode không bổ sung môi trường Win16 |
| Có bộ thay thế InstallShield 3 và launcher 5 | Trang phân phối ToastyTech được công cụ tìm kiếm lập chỉ mục [3] | Chỉ dùng gói khớp phiên bản nhận diện tại máy |
| dgVoodoo hỗ trợ các API DirectDraw và Direct3D cũ | Tài liệu tác giả [4] | Là ứng viên khi đã cài được nhưng lỗi đồ họa |
| DDrawCompat hỗ trợ Windows 11 và DirectX 1–7 | README của tác giả [5] | Một lựa chọn khác để thử riêng biệt |
| cnc-ddraw không hỗ trợ Direct3D | README của tác giả [6] | Không mặc định chọn chỉ vì Hercules trông giống game 2D |
| Bản ISO cụ thể dùng đúng InstallShield 3 | Chưa xác minh tại máy | Đọc metadata, file script và cấu trúc đĩa trước |
| Chạy được đầy đủ gameplay, phim, nhạc trên máy bạn | Chưa thử nghiệm | Cần nghiệm thu riêng từng phần |

Trang ToastyTech không mở được đầy đủ qua công cụ duyệt trong lần nghiên cứu này; phần mô tả InstallShield 3/5 được thấy trong kết quả tìm kiếm. Tài liệu không cung cấp checksum giả định hoặc khẳng định đã kiểm tra binary tải xuống. Claude cần xác minh nguồn và gói thực tế khi triển khai.

Quy trình sau là đề xuất kỹ thuật dựa trên các cơ chế đã được tài liệu hóa, không phải chứng nhận tương thích cho mọi bản Hercules.

## 3 Chuẩn bị môi trường làm việc

Thực hiện trên máy Windows 11 cần chơi game. Dùng phiên Claude Code có quyền truy cập thư mục game trên máy đó; một phiên chỉ chạy trên máy Linux từ xa không thể tự kiểm chứng đồ họa hoặc âm thanh trên Windows của bạn.

Ưu tiên Claude Code native Windows đang có. Tài liệu Anthropic hiện mô tả cách chạy native và WSL; với nhiệm vụ này, native giúp thao tác Windows đơn giản hơn. Không cần cài WSL chỉ để sửa game. [7]

Trong PowerShell, kiểm tra:

```powershell
claude --version
Get-CimInstance Win32_OperatingSystem |
  Select-Object Caption, Version, BuildNumber, OSArchitecture
Get-CimInstance Win32_Processor |
  Select-Object Name, Architecture
Get-CimInstance Win32_VideoController |
  Select-Object Name, DriverVersion
```

Quy trình chính giả định CPU Intel hoặc AMD x64. Nếu máy là ARM64, ghi rõ trong báo cáo và đánh giá lại các DLL cũng như môi trường thực thi; không áp dụng kết luận của nhánh x64 một cách máy móc.

Chọn thư mục ngắn, không có dấu, người dùng có quyền ghi, ví dụ `C:\Games\Hercules-Work`. Nếu tạo ở đó bị từ chối, dùng thư mục khác đã có quyền ghi; không cấp quyền rộng cho toàn ổ đĩa.

| Thư mục | Nội dung |
|---|---|
| `source` | Bản sao toàn bộ nội dung đĩa, giữ nguyên cấu trúc |
| `game` | Game đã cài hoặc đã tái dựng đúng dữ liệu |
| `tools` | Gói setup thay thế hoặc wrapper tải về |
| `backup` | File và cấu hình trước từng thay đổi |
| `logs` | Kiến trúc binary, hash, lỗi và kết quả kiểm tra |
| `scripts` | Script kiểm tra, thiết lập, khởi chạy và hoàn tác |

Giữ nguyên ISO gốc. Chưa chạy setup, autorun hoặc EXE chưa nhận diện chỉ để khám phá cấu trúc.

## 4 Kiểm kê ISO và phân loại binary

### Bước 1 Xác định file thực tế

`Disneys_Hercules_Win_ISO_EN` có thể là tên thư mục hoặc gói nén. Claude phải liệt kê nội dung trước. Phân biệt `.iso`, `.bin` kèm `.cue`, ZIP và thư mục đã giải nén. Không đổi đuôi BIN thành ISO để ép Windows mount.

Với ISO thật, dùng File Explorer → Mount hoặc PowerShell có sẵn. ISO được mount chỉ đọc. [8]

```powershell
# Thay bằng đường dẫn thực tế, không dùng nguyên placeholder.
$IsoPath = 'D:\Downloads\Disneys_Hercules_Win_ISO_EN.iso'
$WorkRoot = 'C:\Games\Hercules-Work'

if (-not (Test-Path -LiteralPath $IsoPath -PathType Leaf)) {
    throw 'Khong tim thay ISO'
}
foreach ($name in @('source','game','tools','backup','logs','scripts')) {
    New-Item -ItemType Directory -Force -Path (Join-Path $WorkRoot $name) |
        Out-Null
}
Get-FileHash -LiteralPath $IsoPath -Algorithm SHA256 |
    Format-List | Out-File (Join-Path $WorkRoot 'logs\iso-hash.txt')

$disk = Get-DiskImage -ImagePath $IsoPath -ErrorAction Stop
if (-not $disk.Attached) {
    $disk = Mount-DiskImage -ImagePath $IsoPath -PassThru -ErrorAction Stop
}
$volumes = @($disk | Get-Volume | Where-Object DriveLetter)
if ($volumes.Count -ne 1) {
    throw 'Can kiem tra volume va ky tu o dia thu cong'
}
$DiscRoot = '{0}:\' -f $volumes[0].DriveLetter
$SourceRoot = Join-Path $WorkRoot 'source'
if (@(Get-ChildItem -LiteralPath $SourceRoot -Force).Count -gt 0) {
    throw 'Thu muc source da co du lieu; kiem tra truoc khi copy lai'
}
Get-ChildItem -LiteralPath $DiscRoot -Force |
    Copy-Item -Destination $SourceRoot -Recurse -Force -ErrorAction Stop
Get-ChildItem -LiteralPath $SourceRoot -Recurse -Force -File |
    Select-Object FullName, Length |
    Export-Csv (Join-Path $WorkRoot 'logs\source-files.csv') -NoTypeInformation
```

Không unmount ngay: game có thể còn cần dữ liệu từ đĩa. Nếu thiết lập thất bại, báo rõ ảnh nào đã mount và để người dùng quyết định giữ hay eject; không tác động ảnh đĩa khác.

### Bước 2 Kiểm tra file gây lỗi và các EXE liên quan

Ghi lại chính xác file bạn đã bấm khi thấy lỗi: autorun, setup hay game. Đọc version information của các EXE; tìm dấu hiệu InstallShield và script cài đặt. Không suy ra phiên bản từ duy nhất một tên như `_INST32I.EX_`.

Claude nên tạo hàm đọc header bằng PowerShell hoặc .NET, không cần cài Python hay Visual Studio. Quy tắc kiểm tra [9]:

1. Kiểm tra kích thước tối thiểu và chữ ký `MZ`.
2. Đọc offset header tại `0x3C`, kiểm tra offset nằm trong file.
3. Nếu chữ ký là `PE\0\0`, đọc COFF Machine và Optional Header Magic.
4. `Machine = 0x014C` với `Magic = 0x010B` là ứng viên PE32 x86.
5. `Machine = 0x8664` với `Magic = 0x020B` là PE32+ x64.
6. Nếu là `NE`, ghi “NE executable, cần xác minh target”; không coi mọi NE là game Win16.
7. Với LE, LX, DOS MZ hoặc header hỏng, ghi đúng loại/unknown, không tự đổi kiến trúc.

Chỉ chữ ký `MZ` không đủ để kết luận “16-bit”, vì PE Windows cũng thường bắt đầu bằng MZ. Không đổi trường Machine trong EXE: đổi nhãn header không dịch mã máy.

### Bước 3 Xuất báo cáo chẩn đoán

Tạo `logs\diagnosis.md` với đường dẫn, kích thước, SHA256, định dạng, Machine, Magic, file version và vai trò dự kiến của từng binary. Đánh dấu giả thuyết riêng với điều đã đọc trực tiếp.

| Kết quả | Hướng tiếp theo |
|---|---|
| Launcher/setup Win16; game x86 | Thay đường cài hoặc cài thủ công có kiểm chứng |
| Game đã có đủ EXE x86 và dữ liệu rời | Thử chạy baseline trước |
| Game chỉ nằm trong archive cài đặt | Cần engine/extractor đúng định dạng |
| Chính game cần Win16 | Dừng nhánh không môi trường Win16; báo giới hạn |
| EXE hỏng, thiếu dữ liệu, khác kiến trúc | Xử lý nguồn đầu vào trước |

## 5 Chọn đường cài tối thiểu

### Nhánh A Dùng hoàn toàn dữ liệu hiện có

Nhánh này đáp ứng chặt nhất yêu cầu không tải thêm, nhưng chỉ khả thi khi dữ liệu có thể dùng trực tiếp hoặc giải nén bằng công cụ đã có.

1. Xác định EXE chính từ metadata, shortcut/script cài đặt và cấu trúc dữ liệu. Không mặc định tên luôn là `HERC.EXE`.
2. Đọc script hoặc manifest để biết file cần chép, thư mục đích và thiết lập cần tạo.
3. Sao chép đúng cây dữ liệu vào `game`. Đừng kéo tất cả file về một thư mục phẳng.
4. Nếu phát hiện archive InstallShield riêng, không mặc định `Expand-Archive` hoặc `expand.exe` giải được. Dừng nhánh A nếu công cụ sẵn có không hỗ trợ.
5. Chỉ tái tạo registry/INI khi đã có bằng chứng chính xác về key, tên value, kiểu dữ liệu và giá trị. Không bịa `CDPath`, `InstallPath`, ProductID hoặc serial.
6. Thử chạy EXE với working directory bằng thư mục chứa game.

**Điều kiện qua bước:** game đi vào menu và load được màn chơi. Một cửa sổ vừa xuất hiện chưa chứng minh cài thủ công đã đầy đủ.

### Nhánh B Thay bộ khởi động cài đặt

Khi xác minh được InstallShield 3, dùng engine thay thế tương ứng; với InstallShield 5 dùng launcher đúng dòng đó. Trang ToastyTech phân biệt các gói này. [3]

1. Tìm gói đúng từ trang nguồn, ghi URL, thời điểm và tên phiên bản.
2. Tải vào `tools`; kiểm tra archive chứa file dự kiến. Lưu SHA256 để tái lập lần sau. Hash tự tính chưa phải xác minh độ tin cậy nếu không có hash công bố để đối chiếu.
3. Với nhánh InstallShield 3 và gói có `setup32.exe`, đặt file đó cạnh setup gốc trong bản sao `source`. Không ghi vào ISO và không xóa setup gốc.
4. Chạy file thay thế từ đúng thư mục chứa dữ liệu setup. Không đoán tham số silent.
5. Chọn cài game vào thư mục `game` đã chọn. Nếu bộ cài đề nghị cài DirectX/driver cổ, bỏ qua thành phần tùy chọn đó và ghi nhận; không ghi đè thư viện Windows hiện tại.
6. Nếu giao diện bộ cài cần thao tác hoặc UAC xuất hiện, người dùng hoàn tất phần đó. Claude Code CLI không mặc nhiên có khả năng bấm mọi cửa sổ Windows.
7. Sau cài đặt, kiểm kê EXE chính và dữ liệu một lần nữa.

Nếu engine báo sai format, thiếu file hay không khởi động: kiểm tra đúng phiên bản và cấu trúc thư mục trước, không thử hàng loạt setup tải ngẫu nhiên. Nhánh này dùng công cụ ngoài trong lúc cài; không được mô tả là hoàn toàn không tải thêm.

## 6 Thử chạy nguyên bản trước khi thêm bản vá

Tắt game trước khi thay đổi cấu hình. Lần thử đầu giữ EXE gốc, không wrapper, không nhiều Compatibility Mode cùng lúc.

```powershell
# Thay ten EXE bang ten da xac minh.
$GameExe = 'C:\Games\Hercules-Work\game\TEN_EXE_THUC_TE.exe'
if (-not (Test-Path -LiteralPath $GameExe -PathType Leaf)) {
    throw 'Chua xac dinh EXE game'
}
$GameDir = Split-Path -Parent $GameExe
Start-Process -FilePath $GameExe -WorkingDirectory $GameDir -PassThru
```

Ghi lại thời điểm launch, có menu không, có load màn không, lỗi nguyên văn và hiện tượng hình/âm thanh. Nếu crash, đọc Application log quanh thời điểm thử bằng `Get-WinEvent`, đối chiếu tên process/faulting module; không kết luận mọi Event ID 1000 đều do game.

Nếu chạy tốt, bỏ qua phần sửa đồ họa. Không thêm “tối ưu” chỉ vì có sẵn.

Nếu cần thử tùy chọn Windows, thay đúng một mục mỗi lần qua Properties → Compatibility: compatibility version, tắt fullscreen optimizations, hoặc DPI override tùy triệu chứng. Ghi giá trị cũ để hoàn tác. Không bật đồng thời Windows 98 mode, 256 colors, 640×480 và Administrator rồi mất khả năng biết mục nào hữu ích.

## 7 Xử lý đồ họa bằng DLL cạnh game

Phần này chỉ áp dụng khi game x86 đã cài được nhưng có lỗi hiển thị. Hai ứng viên chính là dgVoodoo và DDrawCompat. Chúng không sửa được việc Windows từ chối chạy một EXE Win16.

### Lựa chọn 1 dgVoodoo

Tác giả mô tả dgVoodoo là lớp triển khai API đồ họa cũ trên Direct3D 11/12. DLL DirectX được đặt cục bộ; kiến trúc phải khớp ứng dụng. [4]

Quy trình thử đề xuất:

1. Lấy bản stable từ trang tác giả; lưu gói nguyên bản và version đã dùng.
2. Kiểm tra thư mục phân phối, chọn bộ DLL x86 cho game x86, thường nằm ở `MS\x86`.
3. Đối với nhánh DirectDraw/Direct3D cũ, đối chiếu README của đúng release và chọn `DDraw.dll`; nếu nhánh D3D cũ của game cần `D3DImm.dll`, dùng file cùng release. Không tự chép `D3D9.dll` chỉ vì Windows hiện đại.
4. Sao lưu mọi DLL trùng tên trước khi chép cạnh EXE chính.
5. Dùng cấu hình mặc định làm baseline; lưu cấu hình local vào đúng thư mục game. Chỉ thay từng tùy chọn sau khi có lỗi.
6. Giữ tỷ lệ hiển thị gốc khi thử; chưa ép màn rộng hoặc độ phân giải cao.
7. Thử lại gameplay và ghi kết quả. Nếu kém hơn, khôi phục bộ file trước đó.

Đây là cấu hình thử cần nghiệm thu, không phải khẳng định bản ISO của bạn bắt buộc dùng dgVoodoo.

### Lựa chọn 2 DDrawCompat

README của DDrawCompat chỉ dẫn chép `ddraw.dll` từ release vào cạnh EXE và mô tả log `DDrawCompat-exename.log`. Dự án không hỗ trợ phối hợp tùy tiện với wrapper/hook khác. [5]

Quy trình thử đề xuất: trả thư mục game về baseline → sao lưu file trùng tên → đặt DLL DDrawCompat → launch → xem log và gameplay → giữ hoặc hoàn tác. Nếu không có log, kiểm tra quyền ghi, cấu hình logging và khả năng game chưa tải wrapper; không kết luận ngay DLL hoạt động.

### Khi nào cân nhắc cnc ddraw

Chỉ khi xác nhận game dùng đường render DirectDraw software phù hợp. Tác giả nêu rõ cnc-ddraw không hỗ trợ game Direct3D. [6] Hình thức platformer 2D không đủ chứng minh API bên dưới là DirectDraw-only.

**Quy tắc thử nghiệm:** chỉ một bộ wrapper đồ họa hoạt động mỗi lần. Không chép các DLL này vào `System32` hoặc `SysWOW64`, không chạy `regsvr32` để “đăng ký” chúng.

## 8 Registry và quyền truy cập

Chỉ xử lý registry khi log, script cài hoặc lỗi thực tế chỉ ra nhu cầu. Windows có registry view riêng cho ứng dụng 32-bit tại các nhánh được redirect. [10]

Claude cần đọc đúng view, ví dụ qua `Microsoft.Win32.RegistryKey.OpenBaseKey` với `RegistryView.Registry32`, rồi làm việc với key logic đã xác minh. Không ghép thêm `Wow6432Node` đồng thời ép Registry32.

Trước khi sửa, ghi từng value: có tồn tại hay chưa, value kind, nội dung gốc và thay đổi dự kiến. Khi hoàn tác, khôi phục value cũ hoặc chỉ xóa value mới do quy trình tạo. Không xóa cả nhánh Disney nếu có dữ liệu khác.

Chạy bằng user thường trước. Nếu chỉ lỗi khi ghi file, ưu tiên thư mục game có quyền ghi. Nếu game yêu cầu ghi HKLM, phải xác định đúng hành vi; không cấp quyền Everyone Full Control hoặc chạy toàn bộ Claude với Administrator như một cách sửa mặc định.

Nếu cần một thao tác HKLM có quyền cao, tách thành script nhỏ có nội dung rõ ràng. Sau thiết lập, thử lại khả năng chơi bằng quyền thường và ghi trung thực nếu vẫn cần elevation.

## 9 Đĩa CD phim và nhạc nền

Kiểm tra âm thanh thành các mục riêng: hiệu ứng, lời thoại, nhạc nền, âm thanh phim. Không đánh dấu “audio pass” chỉ vì nghe tiếng đánh kiếm.

Nếu game yêu cầu đĩa, giữ mount ISO và xác minh đường dẫn dữ liệu mà game dùng. Lỗi “insert CD” có thể là thiếu file, sai đường dẫn hoặc kiểm tra media; cần phân loại trước khi sửa. Không mặc định bỏ kiểm tra đĩa là cách sửa tương thích.

Với media có nhiều track, dữ liệu file và audio track là hai phần khác nhau. Tài liệu cdrdao cho thấy việc sao chép mixed-mode cần mô tả track. [11] Vì vậy, cần kiểm tra gói tải về thực tế có CUE/BIN, audio riêng hay chỉ dữ liệu; không suy từ chữ “ISO” trong tên gói rằng nhạc đã đầy đủ.

Nếu nguồn thiếu các track âm thanh, Claude không thể khôi phục bản nhạc gốc chỉ bằng sửa code. Nếu có nhạc ngoài, game cũng phải có cơ chế đọc chúng; chép OGG tùy ý vào thư mục không tự tạo hỗ trợ phát nhạc.

Trường hợp không đạt đủ media trong phạm vi công cụ sẵn có: ghi rõ thiếu phần nào. Phương án nới phạm vi gồm bộ media đầy đủ hoặc bản phát hành chính thức phù hợp. Trang Steam xác nhận Disney’s Hercules có bản phát hành tại đó, nhưng sự tồn tại của trang bán không tự chứng minh máy của bạn sẽ chạy hoàn hảo. [12]

## 10 Đóng gói cách mở game một lần bấm

Mục tiêu là mở game không cần Claude Code và không cần terminal thủ công. Ưu tiên shortcut Windows với Target là EXE đã xác minh và Start in là thư mục chứa game.

Nếu còn phải mount ISO, dùng `Launch-Hercules.ps1` thực hiện tuần tự: kiểm tra đường dẫn → kiểm tra ISO đã mount hay chưa → mount đúng ISO khi cần → kiểm tra media → mở EXE với working directory đúng. Ghi lại ảnh do script tự mount, không eject khi game vẫn còn đọc đĩa và không unmount ảnh vốn do người dùng mount sẵn.

Nếu không cần mount và đã xác minh tên EXE, Claude có thể tạo `Play-Hercules.cmd` ngay cạnh game. Ví dụ sau chỉ là mẫu, phải thay tên placeholder trước khi sử dụng:

```bat
@echo off
setlocal
cd /d "%~dp0"
if not exist "TEN_EXE_THUC_TE.exe" (
  echo Khong tim thay EXE game da cau hinh.
  pause
  exit /b 1
)
start "" /D "%~dp0" "%~dp0TEN_EXE_THUC_TE.exe"
```

Không thay đổi ExecutionPolicy toàn máy chỉ để chạy một script. Dùng cách launch phù hợp với chính sách hiện có; nếu bị chính sách tổ chức chặn, báo cụ thể. Không gọi gói này “portable” nếu vẫn phụ thuộc registry hoặc đường dẫn tuyệt đối trên máy đó.

## 11 Nếu muốn Claude Code tự viết mod thực sự

Chỉ bắt đầu sau khi cách cấu hình và wrapper có sẵn thất bại với một lỗi tái lập rõ ràng. Có ba mức công việc:

| Mức | Nội dung | Phù hợp với mục tiêu hiện tại |
|---|---|---|
| Tự động hóa | Script kiểm tra, cài, chọn wrapper, launch, rollback | Ưu tiên |
| Sửa tương thích riêng | Sửa module nguồn mở hoặc viết shim x86 cho lỗi đã đo được | Chỉ khi có bằng chứng cần thiết |
| Port toàn bộ engine sang x64 | Phân tích sâu engine và dữ liệu, tái dựng hành vi | Không cần chỉ để chơi trên Windows 11 |

Lộ trình mod tùy chỉnh:

1. Chốt SHA256 của EXE và kịch bản gây lỗi; thu faulting module/log trước khi thay đổi.
2. Thu hẹp nguyên nhân: đường dẫn, API đồ họa, timing, nhập liệu, media hay quyền ghi.
3. Nếu có mã nguồn wrapper phù hợp, sửa lỗi tối thiểu trên một nhánh riêng, ghi commit và license. Không mặc định mọi wrapper đều công khai nguồn.
4. Nếu cần shim DLL, build x86 khớp process, xác định đúng exports/calling convention và hành vi forwarding. Không phát sinh DLL rỗng chỉ có cùng tên.
5. Nếu phải patch EXE, chỉ áp dụng cho hash đã biết; kiểm tra byte gốc tại offset trước khi ghi, tạo backup và kiểm tra hash đầu ra. Không đưa offset giả định cho phiên bản chưa phân tích.
6. Chạy lại toàn bộ nghiệm thu; đóng gói patch/config do mình tạo cùng hướng dẫn khôi phục, không tự đưa toàn bộ dữ liệu game vào gói chia sẻ.

Toolchain C/C++ hoặc debugger có thể phát sinh ở nhánh này. Nếu yêu cầu tuyệt đối không cài thêm còn hiệu lực, đó là điểm dừng khi máy chưa có công cụ cần thiết. AI không biến một công việc reverse engineering chưa rõ nguyên nhân thành thao tác đảm bảo thành công.

## 12 Nghiệm thu

Phân biệt `PASS`, `FAIL` và `NOT TESTED`. Claude không được đánh dấu PASS cho hình ảnh, âm thanh hoặc gameplay chỉ dựa vào process còn sống. Khi không có công cụ quan sát tương ứng, người dùng xác nhận phần đó.

| Hạng mục | Cách kiểm tra | Tiêu chí |
|---|---|---|
| Mở game | Đóng hoàn toàn rồi mở lại 3 lần | Đều vào menu |
| Gameplay | Chơi màn đầu ít nhất 10 phút | Di chuyển, nhảy, tấn công hoạt động; không crash |
| Chuyển cảnh | Qua ít nhất một lần load/chuyển màn nếu tiếp cận được | Không treo hoặc thiếu dữ liệu |
| Tốc độ | So sánh cảm nhận và hành vi giữa các lần thử | Không tăng tốc bất thường hoặc giật nghiêm trọng |
| Hình ảnh | Menu và trong màn chơi | Không mất sprite, sai màu hoặc méo tỷ lệ |
| Hiệu ứng | Thao tác có âm thanh | Nghe được và không rè bất thường |
| Nhạc | Kiểm tra riêng tại nơi có nhạc | Có đúng nội dung mong đợi hoặc báo thiếu |
| Phim | Xem đoạn phim có trong bản game | Hình và tiếng hoạt động hoặc ghi lỗi riêng |
| Bàn phím | Kiểm tra phím theo thiết lập của game | Không kẹt phím hoặc mất focus |
| Alt Tab | Chuyển ra/vào 5 lần | Trở lại được, không màn đen kéo dài |
| Thiết lập | Đổi một tùy chọn, thoát và mở lại | Giữ tùy chọn nếu game hỗ trợ |
| Tiến trình | Kiểm tra cơ chế save/password thực tế | Cơ chế của phiên bản đó hoạt động |
| Shortcut | Mở từ Desktop | Đúng working directory và media |
| Không Claude | Thoát Claude rồi mở game | Vẫn chơi được |
| Hoàn tác | Khôi phục thay đổi trên bản thử | Đúng file/config trước can thiệp |

Khởi động lại Windows rồi thử shortcut một lần để phát hiện phụ thuộc mount hoặc biến môi trường chỉ tồn tại trong phiên terminal. Nếu chưa chơi được các màn sau, báo phạm vi kiểm thử thay vì ghi “hoạt động 100%”.

## 13 Chẩn đoán khi không đạt

| Triệu chứng | Kiểm tra trước | Không nên làm ngay |
|---|---|---|
| This app cannot run on your PC | Đúng file bị lỗi, header, kiến trúc, file có hỏng không | Bật mọi Compatibility Mode |
| Bộ cài chạy nhưng game không chạy | EXE game, dữ liệu và registry thực tế | Tiếp tục thay setup |
| Thiếu DLL hoặc lỗi 0xc000007b | Kiến trúc và nguồn của DLL được tải | Chép DLL x64 cho game x86 |
| Màn đen, còn âm thanh | Đường render, wrapper đã load, mode hiển thị | Chồng nhiều wrapper |
| Có hiệu ứng nhưng không nhạc | Audio track và cơ chế phát của bản game | Cài lại driver GPU |
| Insert CD | Mount, dữ liệu, đường dẫn media | Tự động tải EXE thay thế không rõ nguồn |
| Không lưu thiết lập | Quyền ghi file/registry, view và VirtualStore nếu liên quan | Cấp quyền rộng cho cả Windows |
| Crash sau một patch | Rollback đúng patch và so hash | Thêm nhiều patch khác để che lỗi |
| Source là BIN CUE | Kiểm tra tracks và công cụ hiện có | Đổi đuôi thành ISO |

Chỉ xem xét bật DirectPlay hoặc cài runtime phụ khi có lỗi/phụ thuộc xác định yêu cầu nó. Không bật sẵn vì đây là game cũ. Tương tự, không tự áp dụng Large Address Aware hoặc sửa subsystem version khi chưa có lý do kiểm chứng.

## 14 Hoàn tác và bàn giao

Mỗi lần sửa có một bản ghi: thời điểm, đường dẫn, hash trước/sau, file backup và lý do. Snapshot mới không được ghi đè baseline ban đầu.

`Restore-Hercules.ps1` phải:

1. Kiểm tra game đã đóng; không tự kill tiến trình không xác định.
2. Chỉ khôi phục file đã sửa và xóa file do workflow thêm, dựa trên manifest.
3. Nếu file hiện tại đã khác hash mà workflow tạo, báo xung đột và giữ lại để tránh mất thay đổi mới của người dùng.
4. Khôi phục từng registry value và tùy chọn compatibility đã ghi nhận.
5. Giữ nguyên dữ liệu save/password và log nghiệm thu.
6. Nói rõ thay đổi nào do installer tạo mà rollback script không quản lý; dùng uninstaller phù hợp cho phần đó nếu cần.

Bộ bàn giao đề xuất gồm `README.md`, `diagnosis.md`, `test-report.md`, `changes.json`, script thiết lập/launch/rollback và cấu hình wrapper nếu thực sự dùng. Không cần giữ Claude hoạt động để game chạy.

## 15 Master prompt cho Claude Code

Lưu tài liệu này vào thư mục làm việc trên Windows, mở Claude Code tại đó và dán đoạn dưới. Điền SOURCE_PATH và WORK_ROOT bằng đường dẫn thật. `ALLOW_LOCAL_HELPERS = true` cho phép bộ cài thay thế dùng một lần và DLL cạnh game; đặt false nếu bạn muốn tuyệt đối chỉ dùng thứ đã có.

```text
Bạn là kỹ sư tương thích ứng dụng Windows cổ. Hãy thực hiện quy trình trong
Hercules-Windows11-ClaudeCode-Master.md trên máy Windows hiện tại.

Mục tiêu: chạy bản Windows Disney's Hercules từ dữ liệu tôi cung cấp trên
Windows 11 x64; sau thiết lập mở bằng shortcut, không cần Claude hoạt động.

SOURCE_PATH = <đường dẫn ISO hoặc thư mục game thực tế>
WORK_ROOT = <thư mục làm việc có quyền ghi>
ALLOW_LOCAL_HELPERS = true

Ràng buộc:
- Không máy ảo, không cài Windows cũ, không DOSBox hoặc môi trường Win16.
- Giữ ISO và binary gốc. Mọi thử nghiệm trên bản sao có manifest và backup.
- Nếu ALLOW_LOCAL_HELPERS=false, không tải/cài helper hay wrapper; nếu hết
  khả năng trong phạm vi đó, báo nguyên nhân cụ thể thay vì lách ràng buộc.
- Nếu true, được tải bộ cài thay thế đúng phiên bản và wrapper từ nguồn
  tác giả/phân phối đã xác minh; ghi URL, version, SHA256 và license.
- Không thay thư viện trong System32/SysWOW64, không tắt bảo vệ Windows,
  không thay ExecutionPolicy toàn máy, không cấp quyền rộng cho hệ thống.
- Không đoán tên EXE, tên registry value, tham số silent, offset patch,
  checksum hoặc khả năng có nhạc chỉ từ tên gói tải về.
- Không đổi PE Machine để giả vờ chuyển 16/32/64 bit.

Hãy tiến hành:
1. Kiểm tra OS/CPU/GPU và vị trí thực thi. Nếu không chạy trên đúng Windows
   đích, nêu giới hạn; không báo đã kiểm thử game.
2. Kiểm kê dữ liệu; xác định ISO thật hay archive/BIN CUE. Hash nguồn.
3. Đọc header và version của autorun, installer, EXE game, DLL liên quan.
   Phân biệt launcher Win16 với game x86 và file hỏng. Xuất diagnosis.md.
4. Chọn nhánh ít phụ thuộc nhất. Thử dữ liệu rời nếu đủ; nếu chưa đủ và
   được phép dùng helper, xác minh phiên bản InstallShield trước khi chọn
   engine/launcher. Giữ cấu trúc source và dùng working directory đúng.
5. Cài vào thư mục có quyền ghi. Không cài DirectX/driver cổ tùy chọn.
   GUI/UAC cần người dùng thì nêu chính xác thao tác cần thực hiện.
6. Launch baseline không wrapper. Ghi lỗi và sự kiện liên quan.
7. Chỉ khi cần, thử từng bộ dgVoodoo hoặc DDrawCompat riêng biệt. Chọn DLL
   x86 theo process. Không chồng wrapper. Hoàn tác nhánh kém hơn.
8. Chỉ sửa registry/config khi có bằng chứng từ file hoặc hành vi thực tế.
   Dùng registry view đúng; ghi trạng thái trước/sau từng value.
9. Kiểm tra media, phim, hiệu ứng và nhạc riêng. Nếu thiếu dữ liệu nguồn,
   báo đúng giới hạn, không bịa nội dung hay hứa khôi phục bằng AI.
10. Tạo shortcut/launcher một lần bấm. Nếu game cần ISO, xử lý mount có
    kiểm tra trạng thái. Không đụng ảnh đĩa khác hoặc eject khi còn dùng.
11. Tạo Restore-Hercules.ps1 theo manifest, phát hiện xung đột thay đổi.
12. Nghiệm thu theo mục 12. Không đánh dấu PASS cho điều chưa quan sát.
    Nếu thiếu công cụ kiểm tra hình/âm thanh, yêu cầu tôi xác nhận ngắn.

Tự làm các bước đọc, chuẩn bị và sửa có thể hoàn tác trong phạm vi này.
Không dừng ở một kế hoạch chung. Với một bước bị chặn, hoàn thành các
bước độc lập còn làm được rồi chỉ rõ dữ liệu hoặc thao tác còn thiếu.
Không tự nâng sang dự án viết engine hoặc cài toolchain lớn.

Tạo các script chỉ khi nhánh thực tế cần đến; hỗ trợ chạy lại an toàn,
không ghi đè backup và không dùng đường dẫn placeholder trong bản cuối.
Ưu tiên PowerShell/.NET/công cụ đã có.

Bàn giao:
- README.md: mở game như thế nào và phụ thuộc thực tế còn lại.
- logs/diagnosis.md: bằng chứng, giả thuyết và nguyên nhân đã xác nhận.
- logs/test-report.md: PASS/FAIL/NOT TESTED, phạm vi gameplay đã thử.
- changes.json và backup tương ứng.
- Script setup/launch/restore cần thiết, cấu hình wrapper nếu có.

Kết luận phải phân biệt rõ: cài được, mở được menu, chơi được màn đầu,
đầy đủ âm thanh/media và những phần chưa kiểm tra.
```

## 16 Nguồn tham khảo

Các liên kết sau được tra cứu ngày 11 tháng 10 năm 2026. Phiên bản binary và trang tải có thể thay đổi; khi thực hiện cần lưu phiên bản thực tế, không coi tài liệu này là pin một release.

1. Microsoft — [Running 32 bit Applications](https://learn.microsoft.com/en-us/windows/win32/winprog64/running-32-bit-applications).
2. Microsoft — [Windows x64 không hỗ trợ thành phần 16 bit](https://learn.microsoft.com/en-us/troubleshoot/windows-client/application-management/x64-windows-not-support-16-bit-programs).
3. ToastyTech — [32 Bit Setups](https://toastytech.com/files/setup.html). Mô tả gói được kiểm tra qua kết quả tìm kiếm; binary chưa được tải và kiểm chứng trong lần soạn này.
4. Dege — [dgVoodoo General Readme](https://dege.freeweb.hu/dgVoodoo2/ReadmeGeneral/) và [trang dự án](https://dege.freeweb.hu/dgVoodoo2/).
5. narzoul — [DDrawCompat README và releases](https://github.com/narzoul/DDrawCompat).
6. FunkyFr3sh — [cnc ddraw README](https://github.com/FunkyFr3sh/cnc-ddraw).
7. Anthropic — [Claude Code Advanced setup](https://code.claude.com/docs/en/setup).
8. Microsoft — [Mount DiskImage](https://learn.microsoft.com/en-us/powershell/module/storage/mount-diskimage?view=windowsserver2025-ps).
9. Microsoft — [PE Format](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format).
10. Microsoft — [Registry Redirector](https://learn.microsoft.com/en-us/windows/win32/winprog64/registry-redirector).
11. cdrdao — [Audio data và mixed mode CD](https://cdrdao.sourceforge.net/).
12. Disney trên Steam — [Disney’s Hercules](https://store.steampowered.com/app/987400/Disneys_Hercules/).
