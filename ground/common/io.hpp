#ifndef IO_HPP
#define IO_HPP

class io {
public:
    virtual void write(unsigned char) = 0;
    virtual bool read(unsigned char& ch) = 0;
};

#endif
