# Flappy Bird
- Họ và tên: Vũ Quang Huy
- Mã sinh viên: 24022804

## Giới thiệu
- 1 phiên bản clone của Flappy Bird. Người chơi sẽ điểu khiển chú chim bay qua các ống liên tục.

## Cách chơi
- Người chơi ấn Space hoặc nhấn chuột để chú chim bay lên. Mỗi lần bay qua ống thì được cộng 1 điểm. Nếu chạm vào ống hoặc rơi xuống đất thì trò chơi kết thúc.

## Đề xuất ngưỡng điểm: 8
- Game có đồ họa, gameplay cũng như giao diện menu bắt đầu và game over đơn giản.
- Game có đầy đủ âm thanh hiệu ứng (chim bay, cộng điểm, va chạm, rơi) và nhạc nền có thể tắt/ bật bằng nút M.
- Game xử lý các input chuẩn xác, có kiểm tra va chạm giữa chim với đất hoặc các ống.
- Sau mỗi lượt chơi thì hiển thị cả điểm của lượt chơi hiện tại lẫn điểm cao nhất, lưu vào file và load lại khi chơi tiếp. 
- Code được chia nhỏ thành nhiều file, quản lý texture và âm thanh bằng class riêng (texture_manager và audio_manager). Các hằng số cấu hình game và struct dùng chung (như PipePair, Button) được đặt riêng trong file utils, giúp chương trình dễ mở rộng và bảo trì.
