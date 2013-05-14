#ifndef OVERLAY_H
#define OVERLAY_H

#include "downloaders/abstractmapdownloader.h"

#include <QString>
#include <QRectF>
#include <QPolygonF>
#include <QImage>

/* 
 * This class has three coordinate systems:
 * 1. Geographic coordinates (longitude, latitude), aka coordinates, coord    (positive y points up)
 * 2. Position in meters with center (0, 0) at image center aka position, pos (positive y points up)
 * 3. Pixel position in image, same as Qt's pixel coordinate system aka point (positive y points down)
 * Thus coord(lon0, lat0), pos(0, 0), and point(overlayImage.rect().center) are the same center point
 */
class Overlay : public AbstractMapDownloader
{
public:
	Overlay(QString absoluteFilePath);
	bool isValid() const;
	bool makeMap(GeoMap *map);
	QString toString() const;

protected:
	QImage render(QPoint center, QSize size, qreal scale) const;
	// map (coord -> point)
	QPoint coordToPoint(QPointF coord) const;	
	// rotate pos around (lon0, lat0) coord - i.e. (0, 0), by rotation degrees (meters -> meters)
	QPointF rotate(QPointF point) const;
	// orthographic projection (coord -> meters) - pl_PL"rzut prostokątny"
	QPointF orthoProjection(QPointF coord) const;
	// inverse orthographic projection (meters -> coord) - pl_PL"odwrotny rzut prostokątny"
	QPointF inverseOrthoProjection(QPointF point) const;

private:
	bool setError(QString error);

	QString name, href, error;
	// map rectangle (including unused black areas) in meters with center (0, 0) at image center corresponding to image.rect()
	QRectF box;
	// actuall map area (excluding unused black areas) in meters with center (0, 0) at image center
	QPolygonF poly;
	// overlayImage rotation in degrees around center
	qreal rotation;
	// center
	qreal lat0, lon0;
	// radius of the earth [m]
	static const qreal R; 
	// whole overlay image loaded from file
	QImage overlayImage;
};

#endif // OVERLAY_H
