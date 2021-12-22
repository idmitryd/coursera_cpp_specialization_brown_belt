#include <memory>
#include <iostream>
#include "Common.h"

using namespace std;

inline bool IsPointInRectangle(Point p, Size size) {
  return p.x < size.width && p.y < size.height;
}

class Ellipse : public IShape {
public:
  Ellipse() = default;
  Ellipse(Point position, Size size, std::shared_ptr<ITexture> texture) :
    pos(position), size(size), texture(texture) {}

  std::unique_ptr<IShape> Clone() const override {
    return std::make_unique<Ellipse>(pos, size, texture);
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
    for (int im_y = pos.y; im_y < pos.y + size.height && im_y < im_height; ++im_y) {
      for (int im_x = pos.x; im_x < pos.x + size.width && im_x < im_width; ++im_x) {
        int shape_x = im_x - pos.x;
        int shape_y = im_y - pos.y;
        if (IsPointInEllipse({shape_x, shape_y}, size)) {
          if (texture && IsPointInRectangle({shape_x, shape_y}, texture->GetSize()))
            im[im_y][im_x] = texture->GetImage()[shape_y][shape_x];
          else
            im[im_y][im_x] = '.';
        }
      }
    }
  }

private:
  Point pos;
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
    for (int im_y = pos.y; im_y < pos.y + size.height && im_y < im_height; ++im_y) {
      for (int im_x = pos.x; im_x < pos.x + size.width && im_x < im_width; ++im_x) {
        int shape_x = im_x - pos.x;
        int shape_y = im_y - pos.y;
        if (texture && IsPointInRectangle({shape_x, shape_y}, texture->GetSize()))
          im[im_y][im_x] = texture->GetImage()[shape_y][shape_x];
        else
          im[im_y][im_x] = '.';
      }
    }
  }

private:
  Point pos;
  Size size;
  std::shared_ptr<ITexture> texture;
};

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
