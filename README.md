- Trong chương trình, nếu bạn muốn sử dụng cho hiển thị lên màn hình LCD_I2C thì có thể bỏ phần comment của task LCD và những phần liên quan đến nó 
- Đấu nối chân như trong file L2C.c, các chân khác đấu nối tương tự như trong phần khởi tạo của từng khối
- Về encoder 334 là một motor khá dởm do là các chân dùng để hàn khá là yếu nên nếu bạn sử dụng nó thì bạn phải đi dây và đấu nối đúng dây vào chip, ngoài ra bạn có thể thay thế bằng động cơ encoder khác.
- Những thư viện như L2C là do mình lấy từ trên git xuống và không thuộc quyền sở hữu của mình
- Những thư viện các bạn bắt buộc phải lấy về thì mới chạy được code
- Trong dự án này có sử dụng hệ điều hành FreeRTOS trên phần mềm KeilC version 5, bạn cần cài hệ điều hành này trước cho keilC để có thể sử dụng được.
