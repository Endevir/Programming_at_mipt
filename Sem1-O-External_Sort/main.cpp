#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <cstdio>
#include <algorithm>

// Interface

template<typename T>
void serialize(T value, std::ostream& out);

template<typename T>
T deserialize(std::istream& in);

template<typename T>
class SerializeIterator {
public:
    typedef void value_type;
    typedef void difference_type;
    typedef void pointer;
    typedef void reference;
    typedef std::output_iterator_tag iterator_category;

    explicit SerializeIterator(std::ostream& stream);

    SerializeIterator& operator=(const T& value);
    SerializeIterator& operator*() { return *this; } // does nothing
    SerializeIterator& operator++() { return *this;} // does nothing
    SerializeIterator& operator++(int) { return *this; } // does nothing

private:
    std::ostream* stream_;
};

template<typename T>
class DeserializeIterator {
public:
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
    typedef std::input_iterator_tag iterator_category;

    DeserializeIterator();
    explicit DeserializeIterator(std::istream& stream);

    const T& operator*() const;
    const T& operator->() const;
    DeserializeIterator& operator++();
    DeserializeIterator operator++(int);

    bool isEnd() const;

private:
    std::istream* stream_;
    T value_;
    bool isEnd_;
};

template<typename T>
bool operator==(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second);

template<typename T>
bool operator!=(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second);

std::string tempFilename();

template<typename InputIter, typename OutputIter, typename Merger>
class ExternalAlgorithm
{
public:
    typedef typename std::iterator_traits<InputIter>::value_type value_type;

    ExternalAlgorithm(InputIter begin, InputIter end,
                      size_t size, size_t maxObjectsInMemory,
                      OutputIter out);

    ~ExternalAlgorithm();

    void run();
private:
    virtual void prepare(std::vector<value_type>& data) = 0;

    InputIter begin_;
    InputIter end_;
    size_t size_;
    size_t maxObjectsInMemory_;
    OutputIter out_;

    size_t countOfFiles_;
    std::fstream* fstreams_;
    std::vector<std::string> filenames_;
};

template<class T>
struct DeserializerCompare
{
    bool operator()(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second);
};

template<class T>
class SortMerger // Слияние сортировки
{
public:
    explicit SortMerger(const std::vector<DeserializeIterator<T> >& deserializers);

    bool hasNext() const;
    T next();

private:
    std::vector<DeserializeIterator<T> > deserializers_;
};

template<typename InputIter, typename OutputIter>
class ExternalSort : public ExternalAlgorithm<
        InputIter, OutputIter, SortMerger<typename std::iterator_traits<InputIter>::value_type> >
{
public:
    typedef ExternalAlgorithm<
            InputIter, OutputIter, SortMerger<typename std::iterator_traits<InputIter>::value_type> > Base;

    ExternalSort(InputIter begin, InputIter end,
                 size_t size, size_t maxObjectsInMemory,
                 OutputIter out);

private:
    virtual void prepare(std::vector<typename Base::value_type>& container);
};

template<class T>
class ReverseMerger
{
public:
    explicit ReverseMerger(const std::vector<DeserializeIterator<T> >& deserializers);

    bool hasNext() const;
    T next();

private:
    std::vector<DeserializeIterator<T> > deserializers_;
};

template<typename InputIter, typename OutputIter>
class ExternalReverse : public ExternalAlgorithm<
        InputIter, OutputIter, ReverseMerger<typename std::iterator_traits<InputIter>::value_type> >
{
public:
    typedef ExternalAlgorithm<
            InputIter, OutputIter, ReverseMerger<typename std::iterator_traits<InputIter>::value_type> > Base;

    ExternalReverse(InputIter begin, InputIter end,
                    size_t size, size_t maxObjectsInMemory,
                    OutputIter out);

private:
    virtual void prepare(std::vector<typename Base::value_type>& container);
};


// ---------------------------------------------------------------------------------------------------------------------------------------------
// -------------------- Implementation ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
void serialize(T value, std::ostream& out)
{
    out.write(reinterpret_cast<char*>(&value), sizeof(T));
}

template<typename T>
T deserialize(std::istream& in)
{
    T value;
    in.read(reinterpret_cast<char *>(&value), sizeof(T));
    return value;
}

template<typename T>
SerializeIterator<T>::SerializeIterator(std::ostream& stream)
{
    stream_ = &stream;
}

template<typename T>
SerializeIterator<T>& SerializeIterator<T>::operator=(const T& value)
{
    assert(stream_ != nullptr);
    serialize(value, *stream_);
    return *this;
}

template<typename T>
DeserializeIterator<T>::DeserializeIterator()
{
    stream_ = nullptr;
    isEnd_ = false;
}

template<typename T>
DeserializeIterator<T>::DeserializeIterator(std::istream& stream)
{
    stream_ = &stream;
    isEnd_ = false;
    ++(*this);
}

template<typename T>
const T& DeserializeIterator<T>::operator*() const
{
    return value_;
}

template<typename T>
const T& DeserializeIterator<T>::operator->() const
{
    return &(operator*());
}

template<typename T>
DeserializeIterator<T>& DeserializeIterator<T>::operator++()
{
    if (isEnd_)
    {
        return *this;
    }
    auto value = deserialize<T>(*stream_);
    if (stream_->eof()) {
        isEnd_ = true;
        return *this;
    }
    value_ = value;
    return *this;
}

template<typename T>
DeserializeIterator<T> DeserializeIterator<T>::operator++(int)
{
    DeserializeIterator ans = *this;
    ++(*this);
    return ans;
}

template<typename T>
bool DeserializeIterator<T>::isEnd() const
{
    return isEnd_;
}

template<typename T>
bool operator==(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second)
{
    return first.isEnd_ == second.isEnd_ && (!first.isEnd_ || first.value_ == second.value_);
}

template<typename T>
bool operator!=(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second)
{
    return first.value_ != second.value_;
}

std::string toString(int a)
{
    std::string ans;
    a = abs(a);
    while (a > 0) {
        ans += static_cast<char>(a % 10 + '0');
        a /= 10;
    }
    return ans;
}

std::string tempFilename()
{
    static int fileNumber = 1;
    std::string str = "TemporaryFile";
    str = str + toString(fileNumber);
    fileNumber++;
    return str;
}

template<typename InputIter, typename OutputIter, typename Merger>
ExternalAlgorithm<InputIter, OutputIter, Merger>::ExternalAlgorithm(InputIter begin, InputIter end, size_t size,
                                   size_t maxObjectsInMemory, OutputIter out)
        : begin_(begin), end_(end), size_(size), maxObjectsInMemory_(maxObjectsInMemory)
        , out_(out), countOfFiles_(0), fstreams_(nullptr)
{
    countOfFiles_ = static_cast<size_t>(ceil(static_cast<double>(size) / static_cast<double>(maxObjectsInMemory)));
    fstreams_ = new std::fstream[countOfFiles_];
    filenames_ = std::vector<std::string>(countOfFiles_);
    for (int i = 0; i < countOfFiles_; i++)
    {
        filenames_[i] = tempFilename();
    }
}

template<typename InputIter, typename OutputIter, typename Merger>
ExternalAlgorithm<InputIter, OutputIter, Merger>::~ExternalAlgorithm()
{
    for (int i = 0; i < countOfFiles_; i++) {
        fstreams_[i].close();
        remove(filenames_[i].c_str());
    }
    delete[] fstreams_;
}

template<typename InputIter, typename OutputIter, typename Merger>
void ExternalAlgorithm<InputIter, OutputIter, Merger>::run()
{
    auto data = std::vector<value_type>(maxObjectsInMemory_);
    size_t elapsedNumbers = size_;

    for (int i = 0; i < countOfFiles_; i++) {
        fstreams_[i].open(filenames_[i], std::fstream::out | std::fstream::binary);
        SerializeIterator<value_type > writer(fstreams_[i]);

        if (elapsedNumbers < data.size())
            data.resize(elapsedNumbers);

        for (int j = 0; j < data.size(); j++)
        {
            data[j] = *begin_;
            ++begin_;
        }
        prepare(data);
        for (int j = 0; j < data.size(); j++)
        {
            writer = data[j];
        }
        elapsedNumbers -= data.size();
        fstreams_[i].close();
    }


    std::vector<DeserializeIterator<value_type > > deserializers(countOfFiles_);

    for (int i = 0; i < countOfFiles_; i++)
    {
        fstreams_[i].open(filenames_[i], std::fstream::in | std::fstream::binary);
        deserializers[i] = DeserializeIterator<value_type >(fstreams_[i]);
    }

    Merger a(deserializers);
    while (a.hasNext())
    {
        out_ = a.next();
    }
}

template<typename T>
bool DeserializerCompare<T>::operator()(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second)
{
    if (first.isEnd() != second.isEnd())
        return first.isEnd() > second.isEnd();
    return *first > *second;
}

template <typename T>
SortMerger<T>::SortMerger(const std::vector<DeserializeIterator<T> > &deserializers)
{
    deserializers_ = deserializers;
    std::make_heap(deserializers_.begin(), deserializers_.end(), DeserializerCompare<T>());
}

template <typename T>
bool SortMerger<T>::hasNext() const
{
    return !deserializers_.empty();
}

template <typename T>
T SortMerger<T>::next()
{
    assert(hasNext());
    T ans = *deserializers_[0];
    std::pop_heap(deserializers_.begin(), deserializers_.end(), DeserializerCompare<T>());

    ++deserializers_[deserializers_.size() - 1];
    if (!deserializers_[deserializers_.size() - 1].isEnd())
    {
        std::push_heap(deserializers_.begin(), deserializers_.end(), DeserializerCompare<T>());
    }
    else
    {
        deserializers_.pop_back();
    }
    return ans;
}

template<typename InputIter, typename OutputIter>
ExternalSort<InputIter, OutputIter>::ExternalSort(InputIter begin, InputIter end,
                 size_t size, size_t maxObjectsInMemory,
                 OutputIter out): Base(begin, end, size, maxObjectsInMemory, out)
{

}

template<typename InputIter, typename OutputIter>
void ExternalSort<InputIter, OutputIter>::prepare(std::vector<typename Base::value_type>& container)
{
    std::sort(container.begin(), container.end());
}


template <typename T>
ReverseMerger<T>::ReverseMerger(const std::vector<DeserializeIterator<T> > &deserializers)
{
    deserializers_ = deserializers;
}

template <typename T>
bool ReverseMerger<T>::hasNext() const
{
    return !deserializers_.empty();
}

template <typename T>
T ReverseMerger<T>::next()
{
    assert(hasNext());
    T ans = *deserializers_[deserializers_.size() - 1];
    ++deserializers_[deserializers_.size() - 1];
    if (deserializers_[deserializers_.size() - 1].isEnd())
        deserializers_.pop_back();
    return ans;
}

template<typename InputIter, typename OutputIter>
ExternalReverse<InputIter, OutputIter>::ExternalReverse(InputIter begin, InputIter end,
                                                  size_t size, size_t maxObjectsInMemory,
                                                  OutputIter out): Base(begin, end, size, maxObjectsInMemory, out)
{

}

template<typename InputIter, typename OutputIter>
void ExternalReverse<InputIter, OutputIter>::prepare(std::vector<typename Base::value_type>& container)
{
    std::reverse(container.begin(), container.end());
}

int main()
{
    std::ifstream ifs("input.txt");
    std::ofstream ofs("output.txt");
    size_t type, count, max;
    ifs >> type >> count >> max;

    if (type == 1)
    {
        ExternalSort<
            std::istream_iterator<int>,
            std::ostream_iterator<int>
        > alg(
            std::istream_iterator<int>(ifs), std::istream_iterator<int>(),
            count, max,
            std::ostream_iterator<int>(ofs, " ")
        );
        alg.run();
    }
    else
    {
        ExternalReverse<
            std::istream_iterator<int>,
            std::ostream_iterator<int>
        > alg(
            std::istream_iterator<int>(ifs), std::istream_iterator<int>(),
            count, max,
            std::ostream_iterator<int>(ofs, " ")
        );
        alg.run();
    }
    return 0;
}