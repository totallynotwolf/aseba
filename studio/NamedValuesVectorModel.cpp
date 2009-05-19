/*
	Aseba - an event-based framework for distributed robot control
	Copyright (C) 2007--2009:
		Stephane Magnenat <stephane at magnenat dot net>
		(http://stephane.magnenat.net)
		and other contributors, see authors.txt for details
		Mobots group, Laboratory of Robotics Systems, EPFL, Lausanne
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any other version as decided by the two original authors
	Stephane Magnenat and Valentin Longchamp.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "NamedValuesVectorModel.h"
#include <QtDebug>
#include <QtGui>

#include <NamedValuesVectorModel.moc>

namespace Aseba
{
	/** \addtogroup studio */
	/*@{*/
	
	NamedValuesVectorModel::NamedValuesVectorModel(NamedValuesVector* namedValues, const QString &tooltipText, QObject *parent) :
		QAbstractTableModel(parent),
		namedValues(namedValues),
		tooltipText(tooltipText)
	{
		Q_ASSERT(namedValues);
	}
	
	NamedValuesVectorModel::NamedValuesVectorModel(NamedValuesVector* namedValues, QObject *parent) :
		QAbstractTableModel(parent),
		namedValues(namedValues)
	{
		Q_ASSERT(namedValues);
	}
	
	int NamedValuesVectorModel::rowCount(const QModelIndex & parent) const
	{
		Q_UNUSED(parent)
		return namedValues->size();
	}
	
	int NamedValuesVectorModel::columnCount(const QModelIndex & parent) const
	{
		Q_UNUSED(parent)
		return 2;
	}
	
	QVariant NamedValuesVectorModel::data(const QModelIndex &index, int role) const
	{
		if (!index.isValid())
			return QVariant();
		
		if (role == Qt::DisplayRole)
		{
			if (index.column() == 0)
				return QString::fromStdString(namedValues->at(index.row()).name);
			else
				return namedValues->at(index.row()).value;
		}
		else if (role == Qt::ToolTipRole && !tooltipText.isEmpty())
		{
			return tooltipText.arg(index.row());
		}
		else
			return QVariant();
	}
	
	QVariant NamedValuesVectorModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
		Q_UNUSED(section)
		Q_UNUSED(orientation)
		Q_UNUSED(role)
		return QVariant();
	}
	
	Qt::ItemFlags NamedValuesVectorModel::flags(const QModelIndex & index) const
	{
		if (index.column() == 0)
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		else
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	}
	
	bool NamedValuesVectorModel::setData(const QModelIndex &index, const QVariant &value, int role)
	{
		Q_ASSERT(namedValues);
		if (index.isValid() && role == Qt::EditRole)
		{
			if (index.column() == 1)
			{
				namedValues->at(index.row()).value = value.toInt();
				emit dataChanged(index, index);
				return true;
			}
		}
		return false;
	}
	
	void NamedValuesVectorModel::addNamedValue(const NamedValue& namedValue)
	{
		Q_ASSERT(namedValues);
		
		unsigned position = namedValues->size();
		
		beginInsertRows(QModelIndex(), position, position);
		
		namedValues->push_back(namedValue);
		
		endInsertRows();
	}
	
	void NamedValuesVectorModel::delNamedValue(int index)
	{
		Q_ASSERT(namedValues);
		Q_ASSERT(index < namedValues->size());
		
		beginRemoveRows(QModelIndex(), index, index);
		
		namedValues->erase(namedValues->begin() + index);
		
		endRemoveRows();
	}
	
	void NamedValuesVectorModel::clear()
	{
		Q_ASSERT(namedValues);
		
		if (namedValues->size() == 0)
			return;
		
		beginRemoveRows(QModelIndex(), 0, namedValues->size()-1);
		namedValues->clear();
		endRemoveRows();
	}
	
	/*@}*/
}; // Aseba
