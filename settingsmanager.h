#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QVariant>

class QSettings;

class QButtonGroup;
class QCalendarWidget;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QDateTimeEdit;
class QDial;
class QDoubleSpinBox;
class QFontComboBox;
class QGroupBox;
class QLineEdit;
class QPlainTextEdit;
class QSlider;
class QSpinBox;
class QTextEdit;
class QTimeEdit;
class QWidget;
class QMainWindow;

class SettingsManager
{

	class AbstractInput
	{
	public:
		AbstractInput(QString name, QObject *receiver, const char *member, QVariant defaultVal);
		virtual ~AbstractInput();
		virtual QVariant toVariant() const = 0;
		virtual void fromVariant(QVariant variant) = 0;

		const QString key;
		QObject *const receiver;
		const char *const member;
		const QVariant defaultVal;
		bool wasChanged;

	protected:
		QByteArray encode(QString pass) const;
		QString decode(QByteArray pass) const;

	private:
		const QByteArray pasKey;
	};

	template <typename T>
	class Input : public AbstractInput
	{
	public:
		Input(QString key, T *object, QObject *receiver, const char *member, QVariant defaultVal):
			AbstractInput(key, receiver, member, defaultVal), object(object) {}
		inline QVariant toVariant() const
		{
			return *object;
		}
		inline void fromVariant(QVariant variant)
		{
			*object = variant.value<T>();
		}
	private:
		T *const object;
	};


	template <typename V>
	class AbstractWrapper
	{
	public:
		AbstractWrapper(AbstractInput *input):
			input(input) {}
		virtual ~AbstractWrapper() {}
		virtual V value() const = 0;
		inline virtual bool wasChanged() const
		{
			return input->wasChanged;
		}
	protected:
		AbstractInput *input;
	};

	template <typename V>
	class DirectWrapper : public AbstractWrapper<V>
	{
	public:
		DirectWrapper(AbstractInput *input):
			AbstractWrapper<V>(input) {}
		inline V value() const
		{
			return this->input->toVariant().template value<V>();
		}
	};

	template <typename V, typename O>
	class DelegateWrapper : public AbstractWrapper<V>
	{
	public:
		DelegateWrapper(AbstractInput *input, O *object, V (O::*method)() const):
			AbstractWrapper<V>(input), object(object), method(method) {}
		inline V value() const
		{
			return (object->*method)();
		}
	private:
		O *object;
		V (O::*method)() const;
	};

protected:

	template <typename V>
	class Field
	{
	public:
		Field():
			wrapper(0) {}
		virtual ~Field()
		{
			if (wrapper)
				delete wrapper;
		}
		inline void init(AbstractInput *input)
		{
			wrapper = new DirectWrapper<V>(input);
		}
		template<typename O>
		inline void init(AbstractInput *input, O *object, V (O::*method)() const)
		{
			wrapper = new DelegateWrapper<V, O>(input, object, method);
		}
		inline V value() const
		{
			return wrapper->value();
		}
		inline operator V() const
		{
			return wrapper->value();
		}
		inline bool wasChanged() const
		{
			return wrapper->wasChanged();
		}
	private:
		AbstractWrapper<V> *wrapper;
	};

public:
	SettingsManager(QSettings &settings);
	~SettingsManager();

	template <typename T>
	inline AbstractInput *makeInput(QString key, T *object, QObject *receiver, const char *member, QVariant defaultVal = QVariant())
	{
		return (inputs << new Input<T>(key, object, receiver, member, defaultVal)).last();
	}

	template <typename T>
	inline AbstractInput *makeInput(QString key, T *object, QVariant defaultVal = QVariant())
	{
		return (inputs << new Input<T>(key, object, 0, 0, defaultVal)).last();
	}

public:
	void load(AbstractInput *singleInput = 0);
	void save(AbstractInput *singleInput = 0);

private:
	QSettings &settings;
	QList<AbstractInput *>inputs;
};

// the resulting array length is the same as a1 lenght
QByteArray operator^(const QByteArray &a1, const QByteArray &a2);


template<>QVariant SettingsManager::Input<QButtonGroup>::toVariant() const;
template<>void SettingsManager::Input<QButtonGroup>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QCalendarWidget>::toVariant() const;
template<>void SettingsManager::Input<QCalendarWidget>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QCheckBox>::toVariant() const;
template<>void SettingsManager::Input<QCheckBox>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QComboBox>::toVariant() const;
template<>void SettingsManager::Input<QComboBox>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QDateEdit>::toVariant() const;
template<>void SettingsManager::Input<QDateEdit>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QDateTimeEdit>::toVariant() const;
template<>void SettingsManager::Input<QDateTimeEdit>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QDial>::toVariant() const;
template<>void SettingsManager::Input<QDial>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QDoubleSpinBox>::toVariant() const;
template<>void SettingsManager::Input<QDoubleSpinBox>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QFontComboBox>::toVariant() const;
template<>void SettingsManager::Input<QFontComboBox>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QGroupBox>::toVariant() const;
template<>void SettingsManager::Input<QGroupBox>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QLineEdit>::toVariant() const;
template<>void SettingsManager::Input<QLineEdit>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QPlainTextEdit>::toVariant() const;
template<>void SettingsManager::Input<QPlainTextEdit>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QSlider>::toVariant() const;
template<>void SettingsManager::Input<QSlider>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QSpinBox>::toVariant() const;
template<>void SettingsManager::Input<QSpinBox>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QTextEdit>::toVariant() const;
template<>void SettingsManager::Input<QTextEdit>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QTimeEdit>::toVariant() const;
template<>void SettingsManager::Input<QTimeEdit>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QWidget>::toVariant() const;
template<>void SettingsManager::Input<QWidget>::fromVariant(QVariant variant);
template<>QVariant SettingsManager::Input<QMainWindow>::toVariant() const;
template<>void SettingsManager::Input<QMainWindow>::fromVariant(QVariant variant);


/*
template<>QVariant SettingsManager::Field<>::toVariant() const;
template<>void SettingsManager::Field<>::fromVariant(QVariant variant);
*/



#endif // SETTINGSMANAGER_H












