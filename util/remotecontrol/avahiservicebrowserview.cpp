#include "avahiservicebrowserview.h"
#include "qavahiservicebrowsermodel.h"

AvahiServiceBrowserView::AvahiServiceBrowserView(QWidget *parent)
    : QTreeView(parent)
{
    m_browserModel = new QAvahiServiceBrowserModel(this);
    m_browserModel->setServiceType("_qmh._tcp");
    m_browserModel->browse();
    setModel(m_browserModel);
    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(handleActivated(QModelIndex)));
}

AvahiServiceBrowserView::~AvahiServiceBrowserView()
{
}

void AvahiServiceBrowserView::handleActivated(const QModelIndex &index)
{
    QAvahiServiceBrowserModel::Service service = m_browserModel->serviceFromIndex(index);
    emit serviceSelected(service.address, service.port);
    hide();
}
