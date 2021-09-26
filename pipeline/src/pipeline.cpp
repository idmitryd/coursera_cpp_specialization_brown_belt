#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from = "";
  string to = "";
  string body = "";
};


class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const {
    if (next_) {
      next_->Process(move(email));
    }
  }
  unique_ptr<Worker> next_;

public:
  void SetNext(unique_ptr<Worker> next) {
    if (!next_) {
      next_ = move(next);
    }
    else {
      next_->SetNext(move(next));
    }
  }
};


class Reader : public Worker {
public:
  Reader (istream& is) : is_(is)
  {
  }
  virtual void Process(unique_ptr<Email> email) override {
  }

  virtual void Run() override {
    string s;
    while(true) {
      unique_ptr<Email> email = make_unique<Email>();
      getline(is_, email->from);
      getline(is_, email->to);
      getline(is_, email->body);
      if (is_)
        PassOn(move(email));
      else
        return;
    }
  }

private:
  istream& is_;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

public:
  Filter(Function pred) : pred(pred) {}

  virtual void Process(unique_ptr<Email> email) override {
    if (pred(*email.get())) {
      PassOn(move(email));
    }
  }
private:
  Function pred;
};


class Copier : public Worker {
public:
  Copier (string to) : to(move(to)) {}

  virtual void Process(unique_ptr<Email> email) override {
    if (email->to == to) {
      PassOn(move(email));
    }
    else {
      unique_ptr<Email> copy = make_unique<Email>(*email);
      copy->to = to;
      PassOn(move(email));
      PassOn(move(copy));
    }
  }
private:
  string to;
};


class Sender : public Worker {
public:
  Sender (ostream& out) : out(out) {}

  virtual void Process(unique_ptr<Email> email) override {
    out << email->from << '\n'
        << email->to   << '\n'
        << email->body << '\n';
    PassOn(move(email));
  }

private:
  ostream& out;
};


// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in) : chain(make_unique<Reader>(in)) {
  }

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter) {
    chain->SetNext(make_unique<Filter>(move(filter)));
    return *this;
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(string recipient) {
    chain->SetNext(make_unique<Copier>(move(recipient)));
    return *this;
  }

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out) {
    chain->SetNext(make_unique<Sender>(out));
    return *this;
  }

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build() {
    return move(chain);
  }
private:
  unique_ptr<Worker> chain;
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
