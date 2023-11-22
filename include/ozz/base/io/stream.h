//----------------------------------------------------------------------------//
//                                                                            //
// ozz-animation is hosted at http://github.com/guillaumeblanc/ozz-animation  //
// and distributed under the MIT License (MIT).                               //
//                                                                            //
// Copyright (c) Guillaume Blanc                                              //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

#ifndef OZZ_OZZ_BASE_IO_STREAM_H_
#define OZZ_OZZ_BASE_IO_STREAM_H_

// Provides Stream interface used to read/write a memory buffer or a file with
// Crt fread/fwrite/fseek/ftell like functions.

#include <cstddef>

#include "ozz/base/containers/vector.h"
#include "ozz/base/platform.h"
#include "ozz/base/span.h"

namespace ozz {
namespace io {

// Declares a stream access interface that conforms with CRT FILE API.
// This interface should be used to remap io operations.
class OZZ_BASE_DLL Stream {
 public:
  // Tests whether a file is opened.
  virtual bool opened() const = 0;

  // Reads _size bytes of data to _buffer from the stream. _buffer must be big
  // enough to store _size bytes. The position indicator of the stream is
  // advanced by the total amount of bytes read.
  // Returns the number of bytes actually read, which may be less than _size.
  virtual size_t Read(void* _buffer, size_t _size) = 0;

  // Writes _size bytes of data from _buffer to the stream. The position
  // indicator of the stream is advanced by the total number of bytes written.
  // Returns the number of bytes actually written, which may be less than _size.
  virtual size_t Write(const void* _buffer, size_t _size) = 0;

  // Declares seeking origin enumeration.
  enum Origin {
    kCurrent,  // Current position of the stream pointer.
    kEnd,      // End of stream.
    kSet,      // Beginning of stream.
  };
  // Sets the position indicator associated with the stream to a new position
  // defined by adding _offset to a reference position specified by _origin.
  // Returns a zero value if successful, otherwise returns a non-zero value.
  virtual int Seek(int _offset, Origin _origin) = 0;

  // Returns the current value of the position indicator of the stream.
  // Returns -1 if an error occurs.
  virtual int Tell() const = 0;

  // Returns the current size of the stream.
  virtual size_t Size() const = 0;

 protected:
  Stream() {}

  // Required virtual destructor.
  virtual ~Stream() {}

 private:
  Stream(const Stream&);
  void operator=(const Stream&);
};

// Implements Stream of type File.
class OZZ_BASE_DLL File : public Stream {
 public:
  // Test if a file at path _filename exists.
  // Note that this function is costly. If you aim to open the file right after,
  // then open it and use File::opened() to test if it's actually existing.
  static bool Exist(const char* _filename);

  // Open a file at path _filename with mode * _mode, in conformance with fopen
  // specifications.
  // Use opened() function to test opening result.
  File(const char* _filename, const char* _mode);

  // Gives _file ownership to the FileStream, which will be in charge of closing
  // it. _file must be nullptr or a valid std::FILE pointer.
  explicit File(void* _file);

  // Close the file if it is opened.
  virtual ~File() override;

  // Close the file if it is opened.
  void Close();

  // See Stream::opened for details.
  virtual bool opened() const override;

  // See Stream::Read for details.
  virtual size_t Read(void* _buffer, size_t _size) override;

  // See Stream::Write for details.
  virtual size_t Write(const void* _buffer, size_t _size) override;

  // See Stream::Seek for details.
  virtual int Seek(int _offset, Origin _origin) override;

  // See Stream::Tell for details.
  virtual int Tell() const override;

  // See Stream::Tell for details.
  virtual size_t Size() const override;

 private:
  // The CRT file pointer.
  void* file_;
};

// Implements an in-memory Stream. Buffer is provided at construction time and
// can be reallocated if overriden. The opening mode is equivalent to fopen w+b
// (binary read/write).
class OZZ_BASE_DLL SpanStream : public Stream {
 public:
  // Construct an empty memory stream opened in w+b mode.
  SpanStream(ozz::span<byte> _buffer);

  // Closes the stream and deallocates memory buffer.
  virtual ~SpanStream();

  // See Stream::opened for details.
  virtual bool opened() const override;

  // See Stream::Read for details.
  virtual size_t Read(void* _buffer, size_t _size) override;

  // See Stream::Write for details.
  virtual size_t Write(const void* _buffer, size_t _size) override;

  // See Stream::Seek for details.
  virtual int Seek(int _offset, Origin _origin) override;

  // See Stream::Tell for details.
  virtual int Tell() const override;

  // See Stream::Tell for details.
  virtual size_t Size() const override;

 private:
  // Maximum stream size.
  static const size_t kMaxSize;

  // Asked to resizes buffers _size bytes.
  // Returns true if the buffer can contains _size bytes.
  virtual bool Resize(size_t _size, ozz::span<byte>& _buffer);

  // Buffer of data.
  ozz::span<byte> buffer_;

  // The effective size of the data in the buffer.
  int end_ = 0;

  // The cursor position in the buffer of data.
  int tell_ = 0;
};

// Implements a self-allocated in-memory Stream.
// The opening mode is equivalent to fopen w+b (binary read/write).
class OZZ_BASE_DLL MemoryStream : public SpanStream {
 public:
  // Construct an empty memory stream opened in w+b mode.
  MemoryStream();

  // Closes the stream and deallocates memory buffer.
  virtual ~MemoryStream();

 private:
  // Resizes buffers size to _size bytes. If _size is less than the actual
  // buffer size, then it remains unchanged.
  // Returns true if the buffer can contains _size bytes.
  virtual bool Resize(size_t _size, ozz::span<byte>& _buffer) override;

  // Buffer storage
  ozz::vector<byte> allocation_;
};
}  // namespace io
}  // namespace ozz
#endif  // OZZ_OZZ_BASE_IO_STREAM_H_
