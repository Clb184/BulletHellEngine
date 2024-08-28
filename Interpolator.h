#ifndef INTERPOLATOR_INCLUDED
#define INTERPOLATOR_INCLUDED

template <class T>
class Interpolator {
public:
	Interpolator();
	void Setup(const T& v0, const T& v1);
private:
	T m_Val0;
	T m_Val1;
	T* m_pTargetData;
};

template <class T>
Interpolator<T>::Interpolator() {
	m_pTargetData = nullptr;
}

template <class T>
void Setup(const T& v0, const T& v1){
	m_Val0 = v0;
	m_Val1 = v1;
}


#endif // !INTERPOLATOR_INCLUDED
