#include <memory>
#include <iostream>
#include "Common.h"

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`
class Ellipse : public IShape {
public:
  Ellipse() = default;
  Ellipse(Point position, Size size, std::shared_ptr<ITexture> texture) :
    position(position), size(size), texture(texture) {}

  std::unique_ptr<IShape> Clone() const override {
    return std::make_unique<Ellipse>(position, size, texture);
  }

  void SetPosition(Point p) override {
    position = p;
  }
  Point GetPosition() const override {
    return position;
  }

  void SetSize(Size s) override {
    size = s;
  }
  Size GetSize() const override {
    return size;
  }

  void SetTexture(std::shared_ptr<ITexture> t) override {
    texture = t;
  }
  ITexture* GetTexture() const override {
    return texture.get();
  }

  void Draw(Image& im) const override {
  }

private:
  Point position;
  Size size;
  std::shared_ptr<ITexture> texture;
};

class Rectangle : public IShape {
public:
  Rectangle() = default;
  Rectangle(Point position, Size size, std::shared_ptr<ITexture> texture) :
    pos(position), size(size), texture(texture) {}

  std::unique_ptr<IShape> Clone() const override {
    return std::make_unique<Rectangle>(pos, size, texture);
  }

  void SetPosition(Point p) override {
    pos = p;
  }
  Point GetPosition() const override {
    return pos;
  }

  void SetSize(Size s) override {
    size = s;
  }
  Size GetSize() const override {
    return size;
  }

  void SetTexture(std::shared_ptr<ITexture> t) override {
    texture = t;
  }
  ITexture* GetTexture() const override {
    return texture.get();
  }

  void Draw(Image& im) const override {
    int im_height = im.size();
    int im_width = im[0].size();
    std::cout << "Start draw" << std::endl;
    std::cout << "im_height: " << im_height << std::endl;
    std::cout << "im_width: " <<  im_width << std::endl;
    if(texture) {
      std::cout << "text_height: " << texture->GetSize().height << std::endl;
      std::cout << "text_width: " << texture->GetSize().width << std::endl;
    }
    for (int i = pos.x; i < pos.x + size.width && i < im_width; ++i) {
      for (int j = pos.y; j < pos.y + size.height && j < im_height; ++j) {
        std::cout << "i: " << i << "\nj: " << j << std::endl;
        if (isPointInTexture(i-pos.x, j-pos.y))
          im[j][i] = texture->GetImage()[j-pos.y][i-pos.x];
        else
          im[j][i] = '.';
      }
    }
  }

private:
  Point pos;
  Size size;
  std::shared_ptr<ITexture> texture;

  bool isPointInTexture(int x_pos, int y_pos) const {
    std::cout << "Figure out if (" << x_pos << ", " << y_pos << ") is on the texture" << std::endl;
    if(texture) {
      int texture_widht = texture->GetSize().width;
      int texture_height = texture->GetSize().height;
      return x_pos < texture_widht && y_pos < texture_height;
    }
    return false;
  }
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch(shape_type) {
    case ShapeType::Rectangle:
      return std::make_unique<Rectangle>();
    case ShapeType::Ellipse:
      return std::make_unique<Ellipse>();
    default:
      throw invalid_argument("Unknown shape was provided");
  }
}
