#ifndef FREQUENCYSPECTRUM_H
#define FREQUENCYSPECTRUM_H

#include <QtCore/QList>

/*!
 * \brief FrequencySpectrum Class
 *
 * Represents spectrum as series of elements
 * consisting of frequency, amplitude, and phase
 */
class FrequencySpectrum {
public:
    FrequencySpectrum(int numPoints = 0);

    struct Element {
        Element()
        :   frequency(0.0), amplitude(0.0), phase(0.0), clipped(false)
        { }

        /*!
         * \brief Frequency in Hz
         */
        qreal frequency;


        /*!
         * \brief Amplitude in range [0.0, 1.0]
         */
        qreal amplitude;

        /*!
         * \brief Phase in range [0.0, 2*PI]
         */
        qreal phase;

        bool clipped;
    };

    typedef QList<Element>::iterator iterator;
    typedef QList<Element>::const_iterator const_iterator;

    /*!
     * \brief Reset spectrum
     */
    void reset();

    /*!
     * \brief Operator enabling indexing
     *
     * \param[in] index - index
     * \param[out] Element - element from list
     */
    Element& operator[](int index);

    /*!
     * \brief Operator enabling indexing
     *
     * \param[in] index - index
     * \param[out] Element - element from list
     */
    const Element& operator[](int index) const;

    /*!
     * \brief Iterator of beginning of the class
     *
     * \param[out] iterator - iterator of begin
     */
    iterator begin();

    /*!
     * \brief Iterator of end of the class
     *
     * \param[out] iterator - iterator of end
     */
    iterator end();

    /*!
     * \brief Iterator of beginning of the class
     *
     * \param[out] iterator - iterator of begin
     */
    const_iterator begin() const;

    /*!
     * \brief Iterator of end of the class
     *
     * \param[out] iterator - iterator of end
     */
    const_iterator end() const;

private:

    QList<Element> m_elements;
};

#endif // FREQUENCYSPECTRUM_H
