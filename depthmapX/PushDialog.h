// Copyright (C) 2011-2012, Tasos Varoudis

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "ui_PushDialog.h"

#include <map>

class CPushDialog : public QDialog, public Ui::CPushDialog
{
	Q_OBJECT
public:
    typedef std::map<std::pair<int,int>, std::string> DestNameMap;
    CPushDialog(const DestNameMap& names, const std::string &origin_layer, const std::string& origin_attribute, QWidget *parent = 0);
    const std::pair<int,int>& getSelection() const;
    const std::pair<int,int> invalid_selection;
    bool	m_count_intersections;
    int		m_function;
private:
    int		m_layer_selection;
	QString	m_origin_attribute;
	QString	m_origin_layer;
	void UpdateData(bool value);
	void showEvent(QShowEvent * event);

    const DestNameMap &m_names;


	private slots:
		void OnOK();
		void OnCancel();
};
