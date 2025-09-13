# 📑 Báo cáo tiến độ dự án
1. Thông tin chung
- Tên dự án: AI server chạy trên board LicheeRV Nano
- Người thực hiện: Phạm Đào Nhật Thuyên  
- Giảng viên hướng dẫn: Thầy Cương 
- Thời gian báo cáo: 13/09/2025  

---

2. Mục tiêu dự án
- Xây dựng hệ thống AI server chạy trên board Licheerv Nano.  
- Ứng dụng TDL SDK của Cvitek để triển khai các mô hình AI như:  
- Nhận diện và phát hiện đối tượng bằng YOLOV8  
- Theo dõi đối tượng trong thời gian thực  
- Đếm số lượng đối tượng

---

3. Tiến độ công việc

✅ Đã hoàn thành
- Tìm hiểu phần cứng Licheerv và kiến trúc Cvitek.  
- Cài đặt môi trường build (toolchain, cross-compile).  
- Lấy nguồn video từ stream rtsp của camera ở công ty bằng thư viện ffmpeg.
- Tích hợp mô hình YOLOv8 để phát hiện nhiều đối tượng.  
- Thực hiện decoder, xử lý AI và encoder sử dụng hardware thay vì CPU.

🔄 Đang thực hiện
- Cải tiến chất lượng stream output
- Fix lỗi Error Collecting Frame 

⏳ Chưa thực hiện
- Ứng dụng đếm đối tượng người.

---

4. Hạn chế 
- Tài liệu TDL SDK chủ yếu bằng tiếng Trung, ít nguồn tham khảo tiếng Anh.  
- Một số model khi cross-compile bị lỗi phụ thuộc phiên bản toolchain.  
- Hạn chế tài nguyên phần cứng khiến tốc độ xử lý chưa cao.
- Cấu hình thông số hợp lí với từng ứng dụng 
